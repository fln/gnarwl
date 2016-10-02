#include <stdlib.h>
#include <gdbm.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ldap.h>
#include <lber.h>

#include <conf.h>
#include "config.h"
#include "util.h"
#include "dbaccess.h"
#include "mailhandler.h"

#ifndef __LCLINT__
#include <syslog.h>
#endif

extern struct conf cfg;
extern char* sender;
extern char** receivers;
extern char* subject;
extern int verbose;

/**
 * Cache for file pointed to by cfg.mailheader
 */
char* header=NULL;

/**
 * Cache for file pointed to by cfg.mailfooter
 */
char* footer=NULL;

/**
 * Persistant connection to the LDAP server
 */
LDAP *ldcon;


void dbCacheHF(void) {
  if (header!=NULL || footer!=NULL) return;

  if (cfg.mailheader!=NULL) {
    header=readFile(cfg.mailheader);
  }
  
  if (cfg.mailfooter!=NULL) {
    footer=readFile(cfg.mailfooter);
  }

  if (header==NULL) header=(char*)calloc(1,sizeof(char));
  if (footer==NULL) footer=(char*)calloc(1,sizeof(char));
}

int dbCheck( char* she, char* me) {
  GDBM_FILE dbf=NULL;
  char *fname=NULL;
  datum data;
  datum key;
  time_t t;
  
  // Skip the whole hassle, if the admin feels lucky
  if (cfg.dbexp==0) return TRUE;
  
  fname=(char*)calloc(strlen(me)+strlen(cfg.dbdir)+5,sizeof(char));
  if (fname==NULL) oom();
  strcpy(fname,cfg.dbdir);
  if (fname[strlen(fname)-1]!='/') fname[strlen(fname)]='/';
  strcat(fname,me);
  
  dbf=dbOpen(fname,GDBM_READER);
  if (dbf==NULL) dbf=dbOpen(fname,GDBM_WRCREAT);
  if (dbf==NULL) {
    syslog(LOG_MAIL|LOG_WARNING,"CRIT/IO %s",fname);
    exit(EXIT_FAILURE);
  }
  
  key.dptr=she;
  key.dsize=(int)strlen(she)+1;
  data=gdbm_fetch(dbf,key);
  if (data.dptr==NULL) {
    gdbm_close(dbf);
    free(fname);
    return TRUE;
  }
  gdbm_close(dbf);
  memcpy(&t,data.dptr,sizeof(t));
  free(data.dptr);
  free(fname);
  if (time(NULL)-t>TIMEFACTOR*cfg.dbexp) return TRUE;
  return FALSE;
}

void dbLock( char* he, char* me) {
  GDBM_FILE dbf;
  char *fname;
  datum key;
  datum data;
  time_t ret;
  
  // Skip the whole hassle, if the admin feels lucky
  if (cfg.dbexp==0) return;
  
  fname=(char*)calloc(strlen(me)+strlen(cfg.dbdir)+5,sizeof(char));
  if (fname==NULL) oom();
  strcpy(fname,cfg.dbdir);
  if (fname[strlen(fname)-1]!='/') fname[strlen(fname)]='/';
  strcat(fname,me);

  dbf=dbOpen(fname,GDBM_WRITER);
  
  if (dbf==NULL) {
    syslog(LOG_MAIL|LOG_WARNING,"CRIT/IO error locking '%s' for writing",fname);
    exit(EXIT_FAILURE);
  }
  
  key.dptr=he;
  key.dsize=strlen(he)+1;
  ret=time(NULL);
  data.dptr=(char*)malloc(sizeof(ret));
  if (data.dptr==NULL) oom();
  memcpy(data.dptr,&ret,sizeof(ret));
  data.dsize=(int)sizeof(ret);
  free(fname);
  if (gdbm_store(dbf,key,data,GDBM_REPLACE)!=0) {
    gdbm_close(dbf);
    syslog(LOG_MAIL|LOG_WARNING,"CRIT/IO %s",me);
    exit (EXIT_FAILURE);
  }
  gdbm_close(dbf);
}


GDBM_FILE dbOpen( char* fname, int mode) {
  GDBM_FILE dbf;
  int retrycount=0;
  struct stat fs;
  
  if (fname==NULL) return NULL;
  
  if ( (mode==GDBM_READER) && (stat(fname,&fs)==-1) ) return NULL;
  
  do {
    dbf=gdbm_open(fname,0,mode,cfg.umask,NULL);
    if (dbf!=NULL) break;
    retrycount++;
    srand(time(NULL));
    sleep(1+(int) (10.0*rand()/(RAND_MAX+1.0)));
  }
  while ((errno == EAGAIN) && (retrycount<=10));
  
  return dbf;
}

void dbClose(GDBM_FILE dbf_ptr) {
  if (dbf_ptr==NULL) return;
  gdbm_close(dbf_ptr);
  dbf_ptr=NULL;
}

int dbContains(const char* str_ptr, GDBM_FILE dbf_ptr) {
  datum key;
  
  if ((str_ptr==NULL) || (dbf_ptr==NULL)) return FALSE;
  key.dptr=(char*)str_ptr;
  key.dsize=(int)strlen(key.dptr)+1;
  if (gdbm_exists(dbf_ptr,key)) return TRUE;
  return FALSE;
}


void dbConnect() {
  int rc;

  if (cfg.ca_cert) {
    rc = ldap_set_option(NULL, LDAP_OPT_X_TLS_CACERTFILE, cfg.ca_cert);
    if (rc != LDAP_SUCCESS) {
        syslog(LOG_MAIL|LOG_ERR,"CRIT/LDAP Set option TLS_CACERTFILE failed: %s",ldap_err2string(rc));
        exit(EXIT_FAILURE);
    }
  }

  if (cfg.uri)
    rc = ldap_initialize(&ldcon, cfg.uri);
  else
    ldcon=ldap_init(cfg.server,cfg.port);

  if (ldcon==NULL) {
    syslog(LOG_MAIL|LOG_ERR,"CRIT/LDAP Connection failed");
    exit(EXIT_FAILURE);
  }

  ldap_set_option(ldcon, LDAP_OPT_PROTOCOL_VERSION, &cfg.protver);
  ldap_set_option(ldcon, LDAP_OPT_DEREF, &cfg.deref);

  if (cfg.starttls) {
    rc = ldap_start_tls_s(ldcon, NULL, NULL);
    if (rc != LDAP_SUCCESS) {
        syslog(LOG_MAIL|LOG_ERR,"CRIT/LDAP StartTLS failed: %s",ldap_err2string(rc));
        exit(EXIT_FAILURE);
    }
  }

  rc=ldap_simple_bind_s(ldcon,cfg.uid,cfg.pwd);
  if (rc!=LDAP_SUCCESS) {
    syslog(LOG_MAIL|LOG_ERR,"CRIT/LDAP %s",ldap_err2string(rc));
    exit(EXIT_FAILURE);
  }
}

void dbDisconnect() {
  if (header!=NULL) { free(header); header=NULL;}
  if (footer!=NULL) { free(footer); footer=NULL;}
  ldap_unbind(ldcon);
}

char** dbQuery(const char* mail) {
  
  LDAPMessage *res;
  struct timeval maxwait;
  char *tmp=NULL;
  char **entry;
  char **retbuf;
  int i;
  
  maxwait.tv_sec=LDAPQUERY_MAXWAIT;
  maxwait.tv_usec=0;
  
  
  cpyStr(&tmp,cfg.qfilter);
  expandVars(&tmp,cfg.map_receiver,(char*)mail);
  expandVars(&tmp,cfg.map_sender,sender);  
  
  i=ldap_search_st(ldcon,cfg.base,cfg.scope,tmp,cfg.macro_attr,0,&maxwait,&res);
  if (i!=LDAP_SUCCESS) {
    if (verbose>=LVL_WARN) {
      syslog(LOG_MAIL|LOG_WARNING,"WARN/LDAP Wrong query base?");
    }
    retbuf=(char**)calloc(1,sizeof(char**));
    if (retbuf==NULL) oom();
    return retbuf;
  }
  
  i=ldap_count_entries(ldcon,res);
  retbuf=(char**)calloc(i+1,sizeof(char**));
  if (retbuf==NULL) oom();
  
  if (i==0){
    if (verbose>=LVL_DEBUG)
      syslog(LOG_MAIL|LOG_DEBUG,"DEBUG/LDAP No match: %s",tmp);
    return retbuf;
  }
  
  free(tmp);
  tmp=NULL;
  
  
  dbCacheHF();
  
  res=ldap_first_entry(ldcon,res);
  if (res==NULL) return retbuf;
  i=0;
  
  while(res!=NULL) {
    int count=0;
    char **attr;
    
    entry=ldap_get_values(ldcon,res,cfg.result);
    if (entry!=NULL && entry[0]!=NULL) {
      retbuf[i]=(char*)malloc((strlen(header)+strlen(footer)+strlen(entry[0])+5)*sizeof(char));
      if (retbuf[i]==NULL) oom();
      retbuf[i][0]='\0';
      strcat(retbuf[i],header);
      strcat(retbuf[i],entry[0]);
      strcat(retbuf[i],footer);
      expandVars(&retbuf[i],cfg.map_subject,subject);
      expandVars(&retbuf[i],cfg.map_sender,sender);
      expandVars(&retbuf[i],cfg.map_receiver,(char*)mail);
      
      while(cfg.macro_name[count]!=NULL) {
        attr=ldap_get_values(ldcon,res,cfg.macro_attr[count]);
        if (attr!=NULL && attr[0]!=NULL) {
          expandVars(&retbuf[i],cfg.macro_name[count],attr[0]);
          ldap_value_free(attr);
        }
        else expandVars(&retbuf[i],cfg.macro_name[count],"");
        count++;
      }
      
      ldap_value_free(entry);
      i++;
    }
    res=ldap_next_entry(ldcon,res);
  }
  
  return retbuf;
}
