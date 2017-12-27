#include <stdio.h>
#include <locator.h>
$include sqlca;
$include sqlda;
#include "dbutil.h"

$define ESQL_HIGH_VERSION;

typedef struct  NameList_t
  {
    struct  NameList_t  *next;
    int      stat;
    char      name[16];
  }NameList;

static NameList    *nList=NULL;
static int    is_fork=0;

int dbFreeCursName(char *name)
{
  NameList  *p;

  p=nList;
  while(p!=NULL)
    if (strcmp(p->name,name)==0)
    {
      p->stat=0;
      return(0);
    }
    else p=p->next;
  return(-1);
}

int dbGetNewCursorName(char *cursName)
{
  static  int  n=0;
  NameList  *p;

  p = nList;
  while(p != NULL)
  if (p->stat == 0)
  {
    strcpy(cursName,p->name);
    p->stat = 1;
    return(0);
  }
  else 
    p = p->next;

  sprintf(cursName,"CUR%d",++n);
  p = (NameList *)malloc(sizeof(NameList));
  if (p == NULL)
    return(-1);
  memset(p,0,sizeof(NameList));
  p->next=nList;
  nList=p;
  p->stat=1;
  strcpy(p->name,cursName);

/**
  sprintf(cursName,"CUR%X",cursName);
**/
  return(0);
}

int dbOpen(char *dbName,char *dbPasswd)
{
  $char *name;
  $int  rc;

  name=dbName;
  if ( is_fork == 1)
  {
$ifdef ESQL_HIGH_VERSION;
    rc=sqldetach();
    if(rc!=0)
    {
      DBlog_print(__FILE__, __LINE__,
              "detach datebase error code=%d",SQLCODE);
      return (sqlca.sqlcode) ;
    } ;
$endif;
  }
  else  is_fork = 1;

$ifdef ESQL_HIGH_VERSION;
  EXEC SQL CONNECT TO $name;
$else;
  EXEC SQL DATABASE $name;
$endif;

  return(sqlca.sqlcode);
}

int dbWaitLock(int waitTime)
{
  switch(waitTime)
  {
    case 10:  EXEC SQL SET LOCK MODE TO WAIT 10;
        break;
    case 50:  EXEC SQL SET LOCK MODE TO WAIT 50;
        break;
    case 100:  EXEC SQL SET LOCK MODE TO WAIT 100;
        break;
    default:
      EXEC SQL SET LOCK MODE TO WAIT ;
      break;
  };
  return(sqlca.sqlcode);
}
int dbNoWaitLock()
{
  EXEC SQL SET LOCK MODE TO NOT WAIT;
  return(sqlca.sqlcode);
}

int dbClose()
{
  if(is_fork==1)
  {
$ifdef ESQL_HIGH_VERSION;
    EXEC SQL DISCONNECT current;
$else;
    EXEC SQL CLOSE DATABASE;
$endif;
  }
  else
  {
$ifdef ESQL_HIGH_VERSION;
    EXEC SQL DISCONNECT all;
$else;
    EXEC SQL CLOSE DATABASE;
$endif;
  }
  is_fork=0;
  sqlexit();
  return(sqlca.sqlcode);
}

int dbBeginWork()
{
  EXEC SQL BEGIN WORK;
  return(sqlca.sqlcode);
}

int dbRollback()
{
  EXEC SQL ROLLBACK WORK;
  return(sqlca.sqlcode);
}

int dbCommit()
{
  EXEC SQL COMMIT WORK;
  return(sqlca.sqlcode);
}

int dbCloseSelect(Select_Info *select_info)
{
  $char  *curs;
  curs=select_info->cursor_name;
  EXEC SQL CLOSE $curs;
  EXEC SQL FREE  $curs;
  dbFreeCursName(select_info->cursor_name);
  select_info->cursor_name[0]='\0';
  return(sqlca.sqlcode);
}

int dbFreeLock(Lock_Info *lock_info)
{
  $char  *curs;

  curs=lock_info->cursor_name;
  EXEC SQL CLOSE $curs;
  EXEC SQL FREE  $curs;
  dbFreeCursName(lock_info->cursor_name);
  lock_info->cursor_name[0]='\0';
  return(sqlca.sqlcode);
}


int dbKillSpace(char *str,int size)
{
  int  len;

  len=strlen(str);
  if (len<size)
    memset(str+len,0,size-len);
  else if (len>=size)
  {
     DBlog_print(__FILE__,__LINE__, "dbKillSpace len error [%s] size[%d]",str,size);
    str[size-1]='\0';
    len=size-1;
  }
  while((--len)>=0)
    if (str[len]==' ')
      str[len]='\0';
    else return(len+1);
  return(len+1);
}


int dbErrLog(char *msg, char *filename, int line)
{
  DBlog_print(filename, line, "%s sqlcode[%d] ISAM[%d]",msg, sqlca.sqlcode,sqlca.sqlerrd[1]);
}

