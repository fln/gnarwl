#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include <conf.h>

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#include "config.h"
#include "util.h"

#ifndef __LCLINT__
#include <sys/syslog.h>
#endif


extern int verbose;
extern struct conf cfg;

void expandVars(char** txt, char* s,char *r) {
  int x;
  int ns;
  int sl;
  char *tmp;
  char *tmp2=NULL;
  
  if (((*txt)==NULL) || (s==NULL)) return;
  if (r==NULL) r="";
  
  // Quickfix: If s is/contains a substring of r, bail out before going into
  // an infinite loop
  for(x=0;x<(int)strlen(r);x++) {
    if (!strcasecmp(r+x,s)) {
      return;
    }
  }
  
  cpyStr(&tmp2,*txt);
  
  sl=strlen(tmp2);
  for (x=0;x<sl;x++) {
    if (tmp2[x]==s[0]) {
      if (!strncasecmp(tmp2+x,s,strlen(s))) {
        ns=sl+strlen(r)-strlen(s)+1;
        tmp=(char*)calloc(ns,sizeof(char));
        if (tmp==NULL) oom();
        strncpy(tmp,tmp2,x);
        strcpy(tmp+x,r);
        strcpy(tmp+strlen(tmp),tmp2+x+strlen(s));
        free(tmp2);
        tmp2=(char*)calloc(strlen(tmp)+1,sizeof(char));
        if (tmp2==NULL) oom();
        strcpy(tmp2,tmp);
        sl=strlen(tmp2);
        free(tmp);
      }
    }
  }
  *txt=tmp2;
}


void oom() {
  syslog(LOG_MAIL+LOG_WARNING,"CRIT/SYS Out of memory");
  closelog();
  exit(EXIT_FAILURE);
}


char **splitString(const char* str, int idx, char delim) {
  int x=0;
  int y=0;
  int z=0;
  int len;
  char* tmp;
  char** ret;
  
  for(x=0;x<(int)strlen(str);x++) {
    while(str[x]==delim && x<(int)strlen(str)) x++;
    while(str[x]!=delim && x<(int)strlen(str)) x++;
    z++;
  }
  
  z++;
  ret=(char**)calloc((size_t)z,sizeof(char*));
  if (ret==NULL) oom();
  z=0;
  
  len=strlen(str);
  while(str[len-1]==delim) len--;
  
  for(x=0;x<len;x++) {
    while(str[x]==delim && x<len) x++;
    y=x;
    if(z==idx && idx>-1) x=len;
    while(str[x]!=delim && x<len) x++;
    
    tmp=(char*)calloc((size_t)x-y+3,sizeof(char));
    if (tmp==NULL) oom();
    memcpy(tmp,str+y*sizeof(char),(size_t)x-y);
    ret[z]=tmp;
    z++;
  }
  return ret;
}

char* readFile(char* fname) {
  
  int fd;
  struct stat fs;
  char *buf;
  
  fd=open(fname,O_RDONLY);
  if ( (fd==-1) && (verbose==LVL_WARN) ) {
    syslog(LOG_MAIL+LOG_INFO,"WARN/IO %s",fname);
    return NULL;
  }
  
  if (fstat(fd,&fs)==-1) {
    syslog(LOG_MAIL+LOG_INFO,"WARN/IO %s",fname);
    return NULL;
  }
  
  buf=(char*)calloc((size_t)fs.st_size+1,sizeof(char));
  
  if (buf==NULL) oom();
  if (fs.st_size!=read(fd,buf,(size_t)fs.st_size)) {
    syslog(LOG_MAIL+LOG_INFO,"WARN/IO %s",fname);
    return NULL;
  }
  return buf;
}

void cleanAddress(char** d) {
  int m=0;
  int r=-1;
  int l=-1;
  
  char* s=NULL;
  char** tmp;
  
  s=(char*)calloc(strlen(*d)+1,sizeof(char));
  if (s==NULL) oom();
  strcpy(s,*d);
  
  for(m=0;m<(int)strlen(s);m++)  if (s[m]=='<') { l=m+1; break; }
  for(m=strlen(s);m>=0;m--) if (s[m]=='>') { r=m-1; break; }
    
  for(m=0;m<(int)strlen(s);m++) {
    if (r>-1 && l>-1) {
      if (m<l || m>r) s[m]='*';
    }
    else {
      if (! ( ((s[m]>47)&&(s[m]<58)) || ((s[m]>63)&&(s[m]<91)) || ((s[m]>96)&&(s[m]<123)) || s[m]=='.' || s[m]=='-' || s[m]=='_' || s[m]=='+' || s[m]=='=') ) s[m]='*';
    }
  }
  
  tmp=splitString(s,1,'*');
  m=1;
  while (tmp[m]!=NULL) {free(tmp[m]); m++;}
  free(*d);
  *d=tmp[0];
  if (tmp[0]!=NULL)
    for(m=0;m<(int)strlen(tmp[0]);m++) tmp[0][m]=(char)tolower(tmp[0][m]);
}


void cpyStr(char** dest, const char* src) {
  if (*dest!=NULL) free(*dest);
  *dest=(char*)malloc((strlen(src)+1)*sizeof(char));
  if (*dest==NULL) oom();
  strcpy(*dest,src);
}

void translateString(char** txt) {
#ifdef HAVE_ICONV
  char* in=*txt;
  char* out;
  size_t inlen;
  size_t outlen;
  iconv_t conv;
  char* to;
#endif
    
  if (cfg.charset==NULL) return;

#ifdef HAVE_ICONV
  conv=iconv_open(cfg.charset, LDAP_CHARSET);
  if (conv != (iconv_t) -1) {
    
    
    inlen=strlen(in);
    outlen=inlen;
    to=(char*)calloc(inlen+1,sizeof(char));
    if (to==NULL) oom();
    out=to;
    
    while(inlen > 0 && outlen > 0) {
      if(iconv(conv, &in, &inlen, &out, &outlen) != 0) { in++; inlen--; }
    }
    iconv_close(conv);
    *out='\0';
    free(*txt);
    *txt=to;
  }
  else {
    syslog(LOG_MAIL|LOG_WARNING,"WARN/MAIL charset conversion failed (errno: %s)",strerror(errno));
  }
#endif
}
