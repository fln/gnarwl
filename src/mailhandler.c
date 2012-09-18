#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <gdbm.h>

#ifndef __LCLINT__
#include <syslog.h>
#endif

#include <conf.h>
#include "mailhandler.h"
#include "config.h"
#include "util.h"
#include "dbaccess.h"


/**
 * config structure from config.h
 */
extern struct conf cfg;

/**
 * Verbose logging?
 */
extern int verbose;

/**
 * Filepointer to the blacklist
 */
GDBM_FILE dbf_b=NULL;

/**
 * Filepointer to the mailfilter
 */
GDBM_FILE dbf_f=NULL;


void addAddr(const char* adr) {
  int i=0;

  if (adr==NULL) return;
  while(receivers[i]!=NULL) {
    if(!strcasecmp(receivers[i], adr)) return;
    i++;
  }
  if (dbContains(adr,dbf_b)) {
    mail_status=mail_status|MAIL_BLADDR;
    return;
  }

  if (i>cfg.maxmail) {
    mail_status=mail_status|MAIL_TOOMANY;
    return;
  }
  receivers=(char**)realloc(receivers,(i+2)*sizeof(char**));
  if (receivers==NULL) oom();
  receivers[i]=NULL;
  receivers[i+1]=NULL;
  
  cpyStr(&(receivers[i]),adr);
}

void parseHeader(const char* hl) {
  char** tmp;
  int i=0;

  tmp=splitString(hl,1,':');
  if (tmp[0]==NULL || tmp[1]==NULL) return;
  
  // Cosmetics: Strip leading space of header data
  if (tmp[1][0]==' ') memmove(tmp[1],tmp[1]+sizeof(char),strlen(tmp[1]));
  
  if(!strcasecmp("sender",tmp[0]) && (mail_status&MAIL_PREDEF_SENDER)!=MAIL_PREDEF_SENDER) {
    cleanAddress(&tmp[1]);
    if (tmp[1]==NULL) {
      mail_status=mail_status|MAIL_LACK;
    }
    else { cpyStr(&sender,tmp[1]); }
    mail_status=mail_status|MAIL_HAS_SENDER;
  }
  
  if(!strcasecmp("reply-to",tmp[0]) && (mail_status&MAIL_PREDEF_SENDER)!=MAIL_PREDEF_SENDER && (mail_status&MAIL_HAS_SENDER)!=MAIL_HAS_SENDER) {
    cleanAddress(&tmp[1]);
    if (tmp[1]==NULL) {
      mail_status=mail_status|MAIL_LACK;
    }
    else { cpyStr(&sender,tmp[1]); }
    mail_status=mail_status|MAIL_HAS_REPLYTO;
  }
  
  if(!strcasecmp("from",tmp[0]) && (mail_status&MAIL_PREDEF_SENDER)!=MAIL_PREDEF_SENDER && (mail_status&MAIL_HAS_SENDER)!=MAIL_HAS_SENDER && (mail_status&MAIL_HAS_REPLYTO)!=MAIL_HAS_REPLYTO) {
    cleanAddress(&tmp[1]);
    if (tmp[1]==NULL) {
      mail_status=mail_status|MAIL_LACK;
    }
    else { cpyStr(&sender,tmp[1]); }
  }
  
  if(!strcasecmp("subject",tmp[0])) {
    cpyStr(&subject,tmp[1]);
  }
  
  if(!strcasecmp("message-id",tmp[0])) {
    cpyStr(&messageid,tmp[1]);
  }
  
  while( (mail_status&MAIL_PREDEF_RECEIVER)!=MAIL_PREDEF_RECEIVER && cfg.recv_header[i]!=NULL) {
    if((!strcasecmp(cfg.recv_header[i],tmp[0]) ) ) {
      char** buf=NULL;
      int c=0;
    
      buf=splitString(tmp[1],-1,',');
    
      while(buf[c]!=NULL) {
        cleanAddress(&buf[c]);
        addAddr(buf[c]);
        free(buf[c]);
        c++;
      }
    }
    i++;
  }
  
  free(tmp[0]);
  free(tmp[1]);
}

void readFromSTDIN(void) {
  char ibuf[MAXLINE]; // "inputbuffer"
  char *bbuf=NULL;    // "backbuffer"
  int lc=0;
  
  while (fgets(ibuf,(int)sizeof(ibuf)-1,stdin) && (*ibuf != '\n') && (lc <= cfg.maxheader)) {
    // delete trailing newline character (messes up folded lines and filter)
    ibuf[strlen(ibuf)-1]='\0';
    
    if (dbContains(ibuf,dbf_f)) mail_status=mail_status|MAIL_BADHEADER;
    
    if (bbuf==NULL) cpyStr(&bbuf,ibuf);
    else {
      if (ibuf[0]==' ' || ibuf[0]=='\t') {
        bbuf=(char*)realloc(bbuf,(strlen(bbuf)+strlen(ibuf)+1)*sizeof(char));
        if (bbuf==NULL) oom();
        strcat(bbuf,ibuf+sizeof(char));
      }
      else {
        parseHeader(bbuf);
        free(bbuf); 
        bbuf=NULL;
        cpyStr(&bbuf,ibuf);
      }
    }
    lc++;
  }
  if (bbuf!=NULL) { parseHeader(bbuf); free(bbuf);}
  if (lc>cfg.maxheader) mail_status=mail_status|MAIL_TOOBIG;
  
  // Dummy instruction to empty stdin
  while (fgets(ibuf,(int)sizeof(ibuf)-1,stdin)) lc++;
  return;
}

int receiveMail(char** recv, const char* sndr) {
  
  mail_status=MAIL_NOTSPECIAL;
  messageid=NULL;
  sender=NULL;
  subject=NULL;
  
  cpyStr(&messageid,"No ID found");
  
  receivers=(char**)calloc(1,sizeof(char**));
  if (receivers==NULL) oom();
  
  if (cfg.mfilter!=NULL) {
    dbf_f=dbOpen(cfg.mfilter,GDBM_READER);
    if (dbf_f==NULL && (verbose>=LVL_WARN) ) {
      syslog(LOG_MAIL|LOG_WARNING,"WARN/IO %s",cfg.mfilter);
    }
  }
  
  if (cfg.blist!=NULL) {
    dbf_b=dbOpen(cfg.blist,GDBM_READER);
    if (dbf_b==NULL && (verbose>=LVL_WARN) ) { 
      syslog(LOG_MAIL|LOG_WARNING,"WARN/IO %s",cfg.blist);
    }
  }

  if (recv[0]!=NULL) {
    int i=0;
    mail_status=mail_status|MAIL_PREDEF_RECEIVER;
    while(recv[i]!=NULL) {addAddr(recv[i]);i++;}
  }
  
  if (sndr!=NULL) {
    sender=NULL;
    mail_status=mail_status|MAIL_PREDEF_SENDER;
    cpyStr(&sender,sndr);
  }

  readFromSTDIN();
  
  if (sender==NULL || receivers[0]==NULL) mail_status=mail_status|MAIL_LACK;

  dbClose(dbf_f);
  dbClose(dbf_b);
  
  if (verbose>=LVL_DEBUG) {
    syslog(LOG_MAIL|LOG_DEBUG,"DEBUG/MAIL Code: %d MessageID: %s",mail_status,messageid);
  }
  
  
  if (mail_status>=MAIL_BADHEADER) return FALSE;
  return TRUE;
}

void sendMail(char* addr, char* body) {
  int p[2];
  int c;
  char* tmp=NULL;
  FILE *desc;
  
  if (pipe(p)!=0) {
    syslog(LOG_MAIL|LOG_ERR,"CRIT/MAIL pipe to MTA failed");
    exit(EXIT_FAILURE);
  }
  
  c=fork();
  if (c<0) {
    syslog(LOG_MAIL|LOG_ERR,"CRIT/MAIL couldn't fork MTA");
    exit(EXIT_FAILURE);
  }

  if (c==0) {
    (void)dup2(p[0],0);
    (void)close(p[0]);
    (void)close(p[1]);

    tmp=(char*)calloc((strlen(cfg.mta)+strlen(cfg.mta_opts)+4),sizeof(char));
    if(tmp==NULL) oom();
    strcpy(tmp,cfg.mta);
    tmp[strlen(cfg.mta)]=' ';
    strcat(tmp,cfg.mta_opts);
    
    expandVars(&tmp,cfg.map_sender,sender);
    expandVars(&tmp,cfg.map_receiver,addr);

    (void)execv(cfg.mta,splitString(tmp,-1,' '));
    syslog(LOG_MAIL|LOG_ERR,"CRIT/MAIL execv(3) returned: %s",tmp);
    exit(EXIT_FAILURE);
  }
  close(p[0]);
  desc=fdopen(p[1],"w");
  fprintf(desc,body);
  fclose(desc);
  
  wait(NULL);
  
  if (verbose>=LVL_INFO) {
    syslog(LOG_MAIL|LOG_INFO,"INFO/MAIL sent mail: %s -> %s",addr, sender);
  }
  if(tmp!=NULL) free(tmp);
}
