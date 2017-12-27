#include <stdio.h>
#include "dbutil.h"
#include <string.h>

unsigned long  db_row_process_count;

int dbCursLogErrInfo(DBcurs *curs,char *cmd, const char *file,const int line)
{
   static  char  msg[256];

   dbCursGetErrMsg(curs,msg,256);

   DBlog_print(file,line,"Err[%s]",msg);
   
   return 0;
}

int dbCursLogErr(DBcurs *curs)
{
   dbCursLogErrInfo(curs,"",__FILE__,__LINE__);
  
   return 0;
}

int dbKillSpace(char *str,int size)
{
  int  len;

  len=strlen(str);
  if (len<size)
    memset(str+len,0,size-len);
  while((--len)>=0)
    if (str[len]==' ')
      str[len]='\0';
    else return(len+1);
  return(len+1);
}

int dbCursOpen(DBcurs *curs)
{
  int  r;
  memset(curs,0,sizeof(DBcurs));
  r=oopen(curs,&dbServer.lda,NULL,-1,-1,NULL,-1);
  return(r);
}

int dbCursClose(DBcurs *curs)
{
  int  r;
  r=oclose(curs);
  return(r);
}
int dbCursExec(DBcurs *curs)
{
  int  r;

  db_row_process_count=0;
  r=oexec(curs);
  db_row_process_count=curs->csrrpc;
  return(r);
}

int dbCursFetch(DBcurs *curs)
{
  int  r;
  db_row_process_count=0;
  r=ofetch(curs);
  db_row_process_count=curs->csrrpc;
  return(r);
}
int dbCursStopFetch(DBcurs *curs)
{
  int  r;
  r=ocan(curs);
  return(r);
}
int dbCursGetErrMsg(DBcurs *curs,char *errmsg,int size)
{
  char  buf[1024];
  memset(buf,0,1024);
/*
  oermsg(curs->csrrc,buf);
*/
  sprintf(buf,"DATABASE ERROR:%d",curs->csrrc);
  strncpy(errmsg,buf,size);
  errmsg[size-1]='\0';
  return(0);
}

int dbGetErrMsg(char *errmsg,int size)
{
  char  buf[1024];
  memset(buf,0,1024);
/**
  oermsg(dbServer.lda.csrrc,buf);
**/
  sprintf(buf,"DATABASE ERROR:%d",dbServer.lda.csrrc);
  strncpy(errmsg,buf,size);
  errmsg[size-1]='\0';
  return(0);
}

int dbCursDefineSqlCmd(DBcurs *curs,char *sqlCmd)
{
  int  r;

  r=osql3(curs,sqlCmd,-1);
  return(r);
}
int dbCursDefineValue(DBcurs *curs,char *name,char *value,int size,int type)
{
  int  r;

  r=obndrv(curs,name,-1,value,size,type,
      -1,NULL,NULL,-1,-1);
  return(r);
}
int dbCursDefineRecvValue(DBcurs *curs,int valueN,char *value,int size,int type)
{
  int  r;

  r=odefin(curs,valueN,value,size,type,
      -1,NULL,NULL,-1,-1,
      NULL,NULL);
  return(r);
}
int dbCursDefineRecvValueAndRCode(DBcurs *curs,
    int valueN,char *value,int size,int type,int *rWide,int *rCode)
{
  int  r;

  r=odefin(curs,valueN,value,size,type,
      -1,NULL,NULL,-1,-1,
      rWide,rCode);
  return(r);
}
int dbCursGetRecvValueInfo(DBcurs *curs,int valueN,
    long *dbsize,short *dbtype,char *name,long *nameSize,
    long *dsize,short *perc,short *scale,short *nullok)
{
  int  r;

  r=odsc(curs,valueN,dbsize,dbtype,name,nameSize,
      dsize,perc,scale,nullok);
  return(r);
}

int dbExecSql(char *sqlCmd)
{
  DBcurs  curs;
  int  r;

  db_row_process_count=0;
  r=dbCursOpen(&curs);
  if (r!=0)
    return(r);
  r=dbCursDefineSqlCmd(&curs,sqlCmd);
  if (r!=0)
  {
    dbCursClose(&curs);
    return(r);
  };
  r=dbCursExec(&curs);
  if (r!=0)
  {
    dbCursClose(&curs);
    return(r);
  };
  db_row_process_count=curs.csrrpc;
  r=dbCursClose(&curs);
  return(r);
}

static utoa16(unsigned v,char *buf)
{
  if (v<16)
  {  if (v<10)  *buf++=(char )('0'+v);
      else  *buf++=(char )('A'+v-10);
    *buf='\0';
  }
  else
  {  utoa16(v/16,buf);
    utoa16(v%16,buf+strlen(buf));
  };
}
static ultoa16(unsigned long v,char *buf)
{
  if (v<16)
  {  if (v<10)  *buf++=(char )('0'+v);
      else  *buf++=(char )('A'+v-10);
    *buf='\0';
  }
  else
  {  ultoa16(v/16,buf);
    ultoa16(v%16,buf+strlen(buf));
  };
}

int ora_utoa( unsigned long v ,int len, char *s )
{
    int i , j , k = v ;
    i = len - 1 ;
    memset( s , 'A' , len ) ;
    while( i >= 0 )
    {
        j = k % 64 ;
        k = k / 64 ;
        if( j <= 25 )
        {
            s[i] = j + 'A' ;
        }
        else if( j <= 51 )
        {
            s[i] = j - 26 + 'a' ;
        }
        else if( j <= 61 )
        {
            s[i] = j - 52 + '0' ;
        }
        else if( j == 62 )
            s[i]= '+' ;
        else 
            s[i]= '/' ;

        i-- ;
    }
    return 0 ;
}
/*        ORACLE 10 add by sunjianya  20070719    */
int dbCursGetRowID(DBcurs *curs,char *RowID,int size)
{
    ora_utoa( curs->csrrid.ridtid.tidtrba , 6 , RowID ) ;
    ora_utoa( curs->csrrid.ridtid.tidpid , 3 , RowID+6 ) ;
    ora_utoa( curs->csrrid.ridbrba , 6 , RowID+6+3 ) ;
    ora_utoa( curs->csrrid.ridsqn , 3 , RowID+6+3+6 ) ;
    RowID[18] = 0 ;
    return 0 ;
}

/*
int dbCursGetRowID(DBcurs *curs,char *RowID,int size)
{  
  char  Buf[64];
  char  s[16];
  int  i;
  Buf[0]='\0';
  ultoa16(curs->csrrid.ridbrba,s);
    for (i=0; i<(8-strlen(s)); ++i)
        strcat(Buf,"0");
    strcat(Buf,s);strcat(Buf,".");
  utoa16(curs->csrrid.ridsqn,s);
    for (i=0; i<(4-strlen(s)); ++i)
        strcat(Buf,"0");
    strcat(Buf,s);strcat(Buf,".");
  utoa16(curs->csrrid.ridtid.tidpid,s);
    for (i=0; i<(4-strlen(s)); ++i)
        strcat(Buf,"0");
    strcat(Buf,s);
  strncpy(RowID,Buf,size);
  return(0);
}
*/
