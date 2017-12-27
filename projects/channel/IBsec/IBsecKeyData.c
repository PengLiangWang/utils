#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "filename.h"
#include "IBsecKeyData.h"

#define BLEN   96

static int  secKeyFile = -1;
static int  openF = 0;
static char *ib_key_list = NULL;

static char *get_key_file_env()
{
  if(ib_key_list != NULL)
  {
    return ib_key_list;
  }

  ib_key_list = (char*)getenv(ENV_IBSECKEYDB);

  return ib_key_list;
}

int OpenSecKeyFile()
{
  if(openF == 0)
  {
    secKeyFile = open(GetEtcFileName(get_key_file_env()),O_RDWR);
    if(secKeyFile > 0)
    {
      openF = 1;
      return 0;
    }
  }
  return 0;
}

int OpenSecKeyFileAndClean()
{
  if( openF == 0)
  {
    secKeyFile = open(GetEtcFileName(get_key_file_env()),O_RDWR|O_CREAT|O_TRUNC,0600);
    if(secKeyFile > 0)
    {
      openF = 1;
      return 0;
    }
    return -1;
  }
  return 0;
}

int CloseSecKeyFile()
{
  if(openF == 1)
  {
    close(secKeyFile);
    secKeyFile = -1;
    openF = 0;
  }
  return 0;
}


static int WriteRecord(int n,char *data)
{
  char  buf[BLEN + 1];
  off_t offset;
  int   rv;

  if(strlen(data) > BLEN )
  {
    return -1;
  }
  memset(buf,' ',BLEN - 1);
  buf[BLEN - 1] = '\0';
  strncpy(buf,data,strlen(data));
  offset = n * BLEN;
  offset = lseek(secKeyFile,offset,SEEK_SET);
  if(offset < 0)
  {
    return -1;
  }

  rv = write(secKeyFile,buf,BLEN);
  if(rv <= 0)
  {
    return -1;
  }

  return 0;
}

static int ReadRecord(int n,char *data)
{
  int     rv;
  off_t   offset;

  memset(data,0,BLEN + 1);
  offset = lseek(secKeyFile,n * BLEN,SEEK_SET);
  if(offset < 0)
  {
    return -1;
  }
  
  rv = read(secKeyFile,data,BLEN);
  if(rv == 0)
  {
    return EOF;
  }
  if(rv < 0)
  {
    return -1;
  }
  return 0;
}

int WriteSecKeyM(char *node,char *mark)
{
  int   r;
  char  record[96];
  int   f = 0;

  if (openF == 0)
  {
    r = OpenSecKeyFile();
    if (r != 0)
    {
      goto E;
    }
    f = 1;
  }

  r = WriteRecord(0,"# CREATE BY IBSEC , DON'T EDIT THIS FILE !!!");
  if (r != 0)
  {
    goto E;
  }

  memset(record,0,96);
  sprintf(record,"M %-10s %16s",node,mark);
  r = WriteRecord(1,record);

E:
  if (f == 1)
    CloseSecKeyFile();

  return  r;
}

int WriteSecKeyList(int n,char *node,char *type,
      char *keyState,char *cryMode,char *keyA ,char *keyB)
{
  char  record[96];
  int   r;
  int   f = 0;

  if (openF == 0)
  { 
    r = OpenSecKeyFile();
    if (r != 0)
    {
      goto E;
    }
    f = 1;
  }
  
  memset(record,0,96);
  sprintf(record,"K %-10s %1s %1s %1s %32s %32s",
        node,type,keyState,cryMode,keyA,keyB);
  r = WriteRecord(2 + n,record);

E:
  if (f == 1 )
    CloseSecKeyFile();
  return r;
}

int DeleteSecKeyList(int n,char *node,char *type)
{
  int  r;
  int  f = 0;

  if (openF == 0)
  {
    r = OpenSecKeyFile();
    if (r != 0)
    {
      goto E;
    }
    f = 1;
  }
  
  r = WriteRecord(2+n,"#");
E:
  if (f == 1)
    CloseSecKeyFile();

  return r;
}

static int LoadSecKeyM(int (*lkm)(char *node,char *mark))
{
  int   r;
  char  record[BLEN+1];
  char  mm[8];
  char  node[11];
  char  mark[17];

  r = ReadRecord(1,record);
  if (r != 0)
  {
    return r;
  }

  memset(mm,0,2);
  memset(node,0,11);
  memset(mark,0,17);
  sscanf(record,"%1s %10s %16s",mm,node,mark);
  if (mm[0]!='M' || node[0]=='\0'||mark[0]=='\0')
  {
    return -1;
  }

  if((*lkm)(node,mark) != 0)
  {
    return -1;
  }
 
  return  0;
}

static int LoadSecKeyList(int (*lk)(int n,char *node,char *keytype,
        char *keystat,char *crymode,char *keya,char *keyb))
{
  int   r;
  int   n = 0;
  char  record[BLEN+1];

  struct
  {
    char  mm[2];
    char  node[11];
    char  keytype[2];
    char  keystat[2];
    char  crymode[2];
    char  keya[33];
    char  keyb[33];
  } k;

  while((r = ReadRecord(n+2,record)) == 0)
  {
    memset(&k,0,sizeof(k));
    sscanf(record,"%1s %10s %1s %1s %1s %32s %32s",
        k.mm,k.node,k.keytype,k.keystat,k.crymode,
        k.keya,k.keyb);

    if (k.mm[0]=='#')
      goto NEXT;

    if (k.mm[0]!='K'||k.node[0]=='\0'||k.keytype[0]=='\0')
      goto ERR;

    r = ( * lk ) (n,k.node,k.keytype,k.keystat,k.crymode,k.keya,k.keyb);
    if (r!=0)
      goto ERR;

  NEXT:
    ++n;

  }
  if (r != EOF)
    goto ERR;

  return  0;
ERR:
  return -1;
}

int LoadSecKeyFile(int (*lkm)(char *node,char *mark),
     int (*lk)(int n,char *node,char *keytype,
      char *keystat,char *crymode,char *keya,char *keyb))
{
  int  r;

  r = OpenSecKeyFile();
  if (r != 0)
    return -1;

  if(lkm)
  {
    r = LoadSecKeyM(lkm);
    if (r != 0)
    {
      goto E;
    }
  }

  if( lk )
  {
    r = LoadSecKeyList(lk);
  }
E:
  CloseSecKeyFile();
  return r;
}

