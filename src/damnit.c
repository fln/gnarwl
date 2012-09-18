#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <gdbm.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include <conf.h>
#include "dbaccess.h"
#include "config.h"
#include "util.h"

// How to display data
char *format="%time -> %entry\n";

/**
 * Print usage information
 */
void printUsage() {
  printf("\nDAMNIT(v%s) - DAtabase MAnagement InTerface\n\n\
  Damnit is a tool for listing/editing gnarwl's database files.\n\
  Options:\n\n\
  \t-h\t\t\t print usage information\n\
  \t-d <file> [<value>]\t delete <value> from <file>\n\
  \t-a <file> [<value>]\t add <value> to <file>\n\
  \t-l <file>\t\t list database file\n\
  \t-f <string>\t\t select output format (use with -l)\n\n",VERSION);
  exit(EXIT_SUCCESS);
}

/**
 * Print the contents of a db file (formated)
 * @param key key in the database
 * @param val value in the database
 */
void printFormated(const char* key, const time_t val) {
  char *tmp=NULL;
  char *tmp2=NULL;
  
  cpyStr(&tmp,format);
  cpyStr(&tmp2,ctime(&val));
  if (tmp2[strlen(tmp2)-1]=='\n') tmp2[strlen(tmp2)-1]='\0';
  
  expandVars(&tmp,"%entry",(char*)key);
  expandVars(&tmp,"%time",tmp2);
  expandVars(&tmp,"%tstamp","%d");
  expandVars(&tmp,"\\n","\n");
  expandVars(&tmp,"\\t","\t");
  
  /*
  free(tmp2);
  tmp2=(char*)calloc(strlen(tmp)+2,sizeof(char));
  if(tmp2==NULL) oom();
  strcpy(tmp2,tmp);
  if (tmp2[strlen(tmp2)]!='\n') tmp2[strlen(tmp2)]='\n';
  */
  printf(tmp,(int)val);
  free(tmp);
  free(tmp2);
}

/**
 * List the contents of a db file
 */
void listFile(char *name) {
  GDBM_FILE dbf;
  datum key;
  datum val;
  time_t t;
  
  dbf=dbOpen(name,GDBM_READER);
  
  if (dbf==NULL) {
    printf("IO Error: %s\n",name);
    exit(EXIT_FAILURE);
  }
    
  key = gdbm_firstkey ( dbf );
  while (key.dptr!=NULL) {
    val = gdbm_fetch(dbf,key);
    if (val.dsize==sizeof(time_t)) {
      memcpy(&t,val.dptr,sizeof(t));
      printFormated(key.dptr,t);
    }
    free(val.dptr);
    val.dptr=key.dptr;
    key = gdbm_nextkey(dbf,key);
    free(val.dptr);
  }
  dbClose(dbf);
}

void addToFile(char *fname, char *entry) {
  GDBM_FILE dbf;
  datum key;
  datum val;
  time_t t;
  
  dbf=dbOpen(fname,GDBM_WRCREAT);
  
  if (dbf==NULL) {
    printf("IO error: %s\n",fname);
    exit(EXIT_FAILURE);
  }

  t=time(NULL);
  key.dptr=entry;
  key.dsize=strlen(entry)+1;
  val.dptr=(char*)malloc(sizeof(t));
  if (val.dptr==NULL) {
    printf("Out of memory");
    exit(EXIT_FAILURE);
  }
  memcpy(val.dptr,&t,sizeof(t));
  val.dsize=sizeof(t);
  if (gdbm_store(dbf,key,val,GDBM_REPLACE)!=0) {
    printf("IO error: %s\n",fname);
    dbClose(dbf);
    exit(EXIT_FAILURE);
  }
  dbClose(dbf);
}

void delFromFile(char* fname, char* entry) {
  GDBM_FILE dbf;
  datum key;
  
  dbf=dbOpen(fname,GDBM_WRITER);
  
  if (dbf==NULL) {
    printf("IO error: %s\n",fname);
    exit(EXIT_FAILURE);
  }

  key.dptr=entry;
  key.dsize=strlen(entry)+1;
  if (!gdbm_exists(dbf,key)) {
    printf("Key not in database: %s\n",entry);
    exit(EXIT_FAILURE);
  }
  
  if (gdbm_delete(dbf,key)!=0) {
    printf("IO error: %s",fname);
    dbClose(dbf);
    exit(EXIT_FAILURE);
  }
  dbClose(dbf);
}

int main(int argc,char **argv) {
  int ch;
  char* fname=NULL;
  char* val=NULL;
  char buf[MAXLINE];
  
  setDefaults();
  
  if (argc==1) printUsage();
  
  while ((ch = getopt(argc, argv, "hf:l:a:d:")) != EOF) {
    switch((char)ch) {
      case 'h': printUsage();
      case 'f': {format=optarg;break;}
      case 'l': {
        listFile(optarg);
        exit(EXIT_SUCCESS);
      }
      case 'a': {
        fname=optarg;
        val=argv[optind];
        if (val!=NULL) addToFile(fname,val); 
        else while (fgets(buf,(int)sizeof(buf),stdin) && (*buf != '\n')) {
          if (buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]='\0';
          addToFile(fname,buf);
        }
        exit(EXIT_SUCCESS);
      }
      case 'd': {
        fname=optarg;
        val=argv[optind];
        if (val!=NULL) delFromFile(fname,val);
        else while (fgets(buf,(int)sizeof(buf),stdin) && (*buf != '\n')) {
          if (buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]='\0';
          delFromFile(fname,buf);
        }
        exit(EXIT_SUCCESS);
      }
    }
  }
  return EXIT_SUCCESS;
}
