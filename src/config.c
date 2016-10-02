#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ldap.h>

#ifndef __LCLINT__
#include <syslog.h>
#endif

#include <conf.h>
#include "config.h"
#include "util.h"

#ifndef LDAP_VERSION2
#define PROTVER2 LDAP_PROTOCOL_DETECT
#else 
#define PROTVER2 LDAP_VERSION2
#endif

#ifndef LDAP_VERSION3
#define PROTVER3 LDAP_PROTOCOL_DETECT
#else 
#define PROTVER3 LDAP_VERSION3
#endif


struct conf cfg;
int verbose=LVL_CRIT;

void putEntry(char* key, char* val) {
  
  if (key==NULL || val==NULL) return;
  
  if (!strcasecmp(key,"login")) {
    cfg.uid=val;
    return;
  }
  if (!strcasecmp(key,"password")) {
    cfg.pwd=val;
    return;
  }
  if (!strcasecmp(key,"badheaders")) {
    cfg.mfilter=val;
    return;
  }
  if (!strcasecmp(key,"blacklist")) {
    cfg.blist=val;
    return;
  }
  if (!strcasecmp(key,"forceheader")) {
    cfg.mailheader=val;
    return;
  }
  
  if (!strcasecmp(key,"forcefooter")) {
    cfg.mailfooter=val;
    return;
  }
  
  
  // val may not be NULL below
  
  if (!strcasecmp(key,"map_field")) {
    int i=0;
    char **entry=splitString(val,1,' ');
    
    free(val);
    if (entry[1]==NULL) return;
    
    while(cfg.macro_attr[i]!=NULL) i++;
    cfg.macro_attr=(char**)realloc(cfg.macro_attr,(i+2)*sizeof(char**));
    cfg.macro_name=(char**)realloc(cfg.macro_name,(i+2)*sizeof(char**));
    if (cfg.macro_attr==NULL || cfg.macro_name==NULL) oom();
    cfg.macro_attr[i+1]=NULL;
    cfg.macro_name[i+1]=NULL;
    cfg.macro_attr[i]=entry[1];
    cfg.macro_name[i]=entry[0];
    
    return;
  }
  
  if (!strcasecmp(key,"server_uri")) {
    cfg.uri=val;
    return;
  }
  if (!strcasecmp(key,"ca_cert")) {
    cfg.ca_cert=val;
    return;
  }
  if (!strcasecmp(key,"starttls")) {
    cfg.starttls = atoi(val);
    return;
  }
  if (!strcasecmp(key,"server")) {
    cfg.server=val;
    return;
  }
  if (!strcasecmp(key,"port")) {
    cfg.port=atoi(val);
    return;
  }
  if (!strcasecmp(key,"scope")) {
    if (!strcasecmp(val,"base")) cfg.scope=LDAP_SCOPE_BASE;
    if (!strcasecmp(val,"one")) cfg.scope=LDAP_SCOPE_ONELEVEL;
    if (!strcasecmp(val,"sub")) cfg.scope=LDAP_SCOPE_SUBTREE;
    return;                                                                     
  }
  if (!strcasecmp(key,"base")) {
    cfg.base=val;
    return;
  }
  if (!strcasecmp(key,"queryfilter")) {
    cfg.qfilter=val;
    return;
  }
  if (!strcasecmp(key,"result")) {
    cfg.result=val;
    return;
  }
  if (!strcasecmp(key,"charset")) {
    cfg.charset=val;
    return;
  }
  if (!strcasecmp(key,"map_sender")) {
    cfg.map_sender=val;
    return;
  }
  if (!strcasecmp(key,"map_receiver")) {
    cfg.map_receiver=val;
    return;
  }
  if (!strcasecmp(key,"blockfiles")) {
    cfg.dbdir=val;
    return;
  }
  if (!strcasecmp(key,"blockexpire")) {
    cfg.dbexp=atoi(val);
    return;
  }
  if (!strcasecmp(key,"mta")) {
    char** tmp=splitString(val,1,' ');
    free(val);
    val=NULL;
    if(tmp[0]!=NULL) cfg.mta=tmp[0];
    if(tmp[1]!=NULL) cfg.mta_opts=tmp[1];
    return;
  }
  if (!strcasecmp(key,"recvheader")) {
    int i=0;
    char** tmp=splitString(val,-1,' ');
    
    free(val);
    while(cfg.recv_header[i]!=NULL) {
      free(cfg.recv_header[i]);
      i++;
    }
    free(cfg.recv_header);
    i=0;
    while(tmp[i]!=0) i++;
    cfg.recv_header=(char**)malloc((i+1)*sizeof(char**));
    cfg.recv_header=tmp;
    return;
  }
  if (!strcasecmp(key,"map_subject")) {
    cfg.map_subject=val;
    return;
  }
  if (!strcasecmp(key,"maxreceivers")) {
    cfg.maxmail=atoi(val);
    return;
  }
  if (!strcasecmp(key,"maxheader")) {
    // Adding 2 is a cosmetical fix
    cfg.maxheader=atoi(val)+2;
    return;
  }
  if (!strcasecmp(key,"loglevel")) {
    verbose=atoi(val);
    return;
  }
  if (!strcasecmp(key,"umask")) {
    //val[0]=' ';
    cfg.umask=(int)strtol(val,NULL,8);
    return;
  }
  if (!strcasecmp(key,"protocol")) {
    switch(atoi(val)) {
      case 2: {cfg.protver = LDAP_VERSION2; break;}
      default: {cfg.protver = LDAP_VERSION3; break;}
    }
    return;
  }
  
  if (!strcasecmp(key,"deref")) {
    if (!strcasecmp(val,"never")) cfg.deref=LDAP_DEREF_NEVER;
    if (!strcasecmp(val,"search")) cfg.deref=LDAP_DEREF_SEARCHING;
    if (!strcasecmp(val,"find")) cfg.deref=LDAP_DEREF_FINDING;
    if (!strcasecmp(val,"always")) cfg.deref=LDAP_DEREF_ALWAYS;
    return;
  }

  syslog(LOG_MAIL|LOG_WARNING,"WARN/CFG Unknown config directive: %s",key);
}

void setDefaults(void) {
  cfg.umask=UMASK;
  cfg.base=DEFAULT_BASE;
  cfg.protver=LDAP_VERSION3;
  cfg.uid=NULL;
  cfg.pwd=NULL;
  cfg.uri=NULL;
  cfg.starttls=0;
  cfg.server=DEFAULT_SERVER;
  cfg.qfilter=DEFAULT_QFILTER;
  cfg.mfilter=NULL;
  cfg.scope=LDAP_SCOPE_SUBTREE;
  cfg.port=LDAP_PORT;
  cfg.dbdir=BLOCKDIR;
  cfg.dbexp=DEFAULT_EXPIRE;
  cfg.mta=DEFAULT_MTA;
  cfg.mta_opts="";
  cfg.blist=NULL;
  cfg.charset=NULL;
  cfg.maxmail=DEFAULT_MAIL_LIMIT;
  cfg.maxheader=DEFAULT_MAIL_LIMIT;
  cfg.result=DEFAULT_MES;
  cfg.map_sender=DEFAULT_MAP_SENDER;
  cfg.map_receiver=DEFAULT_MAP_RECEIVER;
  cfg.map_subject=DEFAULT_MAP_SUBJECT;
  cfg.macro_attr=(char**)calloc(1,sizeof(char**));
  cfg.macro_name=(char**)calloc(1,sizeof(char**));
  cfg.recv_header=(char**)calloc(3,sizeof(char**));
  cpyStr(&cfg.recv_header[0],"to");
  cpyStr(&cfg.recv_header[1],"cc");
  if (cfg.macro_attr==NULL || cfg.macro_name==NULL) oom();
  cfg.deref=LDAP_DEREF_NEVER;
}

void readConf(char *cfile) {
  FILE *fptr;
  char buf[MAXLINE];
  char** tmp;
  int pos=0;

  setDefaults();
  fptr=fopen(cfile,"r");
  if (fptr==NULL) {
    syslog(LOG_MAIL|LOG_ERR,"CRIT/IO %s",cfile);
    exit(EXIT_FAILURE);
  }
  while(((fgets(buf, (size_t)sizeof(buf), fptr)) && (buf != NULL))) {
    if (!((buf[0]=='#') || (*buf=='\n'))) { 
      if (buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]='\0';
      tmp=splitString(buf,1,' ');
      //printf("%s:%s\n",tmp[0],tmp[1]);
      putEntry(tmp[0],tmp[1]);
      free(tmp[0]);
    }
  }
  (void)fclose(fptr);
  
  while(cfg.macro_attr[pos]!=NULL) pos++;
  tmp=(char**)calloc(sizeof(char**)*2+pos*sizeof(char**),sizeof(char**));
  if (tmp==NULL) oom();
  memcpy(tmp,cfg.macro_attr,sizeof(char**)*pos);
  tmp[pos]=cfg.result;
  free(cfg.macro_attr);
  cfg.macro_attr=tmp;
}
