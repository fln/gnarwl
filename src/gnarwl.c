#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifndef __LCLINT__
#include <syslog.h>
#endif

#include <conf.h>
#include "util.h"
#include "config.h"
#include "dbaccess.h"
#include "mailhandler.h"

/* 
 * We can't feed addresses from the "-a" switch directly to addAddr(),
 * as cfg.blacklist may not be set up, yet. So we cache them here.
 */
char** tmprecv=NULL;

void printUsage(void) {
  
  printf("\nGNARWL(v%s) - Gnu Neat Auto Reply With LDAP\n\n\
  Read email through stdin, reply to it, if necessary\n\
  Options:\n\n\
  \t-h\t\tprint help\n\
  \t-a <address>\tforce receiver address\n\
  \t-s <address>\tforce sender address\n\
  \t-c <cfgfile>\tuse configfile (default: %s)\n\
  \n",VERSION,CFGFILE);
  
  exit(EXIT_SUCCESS);
}

void tmpAddrStore(const char* adr) {
  int i=0;
  
  if(adr==NULL || adr[0]=='\0') return;
  
  while(tmprecv[i]!=NULL) {
    if(!strcasecmp(tmprecv[i],adr)) return;
    i++;
  }
  
  tmprecv=(char**)realloc(tmprecv,(i+2)*sizeof(char**));
  if(tmprecv==NULL) oom();
  tmprecv[i]=NULL;
  tmprecv[i+1]=NULL;
  cpyStr(&(tmprecv[i]),adr);
  cleanAddress(&(tmprecv[i]));
}


int main(int argc, char **argv) {
  int ch;
  char* cfg_file;
  char** rep;
  char* sndr=NULL;
  extern char** receivers;
  extern int verbose;
  
  cfg_file=CFGFILE;
  tmprecv=(char**)calloc(1,sizeof(char**));
  if (tmprecv==NULL) oom();
  
  while ((ch = getopt(argc, argv, "hc:a:s:")) != EOF) {
    switch((char)ch) {
      case 'h': printUsage(); break;
      case 'c': cfg_file=optarg;break;
      case 'a': tmpAddrStore(optarg);break;
      case 's': cpyStr(&sndr,optarg);cleanAddress(&sndr);break;
    }
  }
  
  openlog("gnarwl",LOG_PID,LOG_MAIL);
  readConf(cfg_file);
  
  if (receiveMail(tmprecv,sndr)==FALSE) return EXIT_SUCCESS;
  
  ch=0;
  dbConnect();

  while(receivers[ch]!=NULL) {
    int i=0;
    rep=dbQuery(receivers[ch]);
    while(rep[i]!=NULL) {
      if (dbCheck(sender,receivers[ch])) {
        translateString(&rep[i]);
        sendMail(receivers[ch],rep[i]);
        dbLock(sender,receivers[ch]);
      }
      else {
        if (verbose>=LVL_DEBUG) {
          syslog(LOG_MAIL|LOG_DEBUG,"DEBUG/MAIL blocked: %s & %s",sender,receivers[ch]);
        }
      }
      free(rep[i]);
      i++;
    }
    ch++;
  }

  dbDisconnect();
  closelog();
  return EXIT_SUCCESS;
}
