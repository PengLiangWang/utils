#include <stdio.h>
#include <stdarg.h>
#include "dbutil.h"


int dbExecSql_va(char *sqlCmd,...)
{
  va_list    args;
  DBcurs    curs;
  int    r;
  char    *valueName;
  char    *valueP;
  int    valueSize;
  int    valueType;

  db_row_process_count=0;
  r=dbCursOpen(&curs);
  if (r!=0)
  {
    dbCursLogErrInfo(&curs,sqlCmd,__FILE__,__LINE__);
    return(r);
  }
  r=dbCursDefineSqlCmd(&curs,sqlCmd);
  if (r!=0)
  {
    dbCursLogErrInfo(&curs,sqlCmd,__FILE__,__LINE__);
    dbCursClose(&curs);
    return(r);
  };

  va_start(args,sqlCmd);
  while((valueName=va_arg(args,char *))!=NULL)
  {
    valueP   = va_arg(args,char *);
    valueSize= va_arg(args,int);
    valueType= va_arg(args,int);

    r=dbCursDefineValue(&curs,valueName,valueP,valueSize,valueType);
    if (r!=0)
    {
      dbCursLogErrInfo(&curs,sqlCmd,__FILE__,__LINE__);
      dbCursClose(&curs);
      va_end(args);
      return(r);
    };
  };

  r=dbCursExec(&curs);
  if (r!=0)
  {
    dbCursLogErrInfo(&curs,sqlCmd,__FILE__,__LINE__);
    dbCursClose(&curs);
    return(r);
  };
  db_row_process_count=curs.csrrpc;
  r=dbCursClose(&curs);
  return(r);
}

int dbCursExecSql_va(DBcurs *curs,char *sqlCmd,...)
{
  va_list    args;
  int    r;
  char    *valueName;
  char    *valueP;
  int    valueSize;
  int    valueType;

  db_row_process_count=0;
  r=dbCursDefineSqlCmd(curs,sqlCmd);
  if (r!=0)
  {
    dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
    return(r);
  };

  va_start(args,sqlCmd);
  while((valueName=va_arg(args,char *))!=NULL)
  {
    valueP   = va_arg(args,char *);
    valueSize= va_arg(args,int);
    valueType= va_arg(args,int);

    r=dbCursDefineValue(curs,valueName,valueP,valueSize,valueType);
    if (r!=0)
    {
      dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
      va_end(args);
      return(r);
    };
  };

  r=dbCursExec(curs);
  db_row_process_count=curs->csrrpc;
  if (r!=0)
    dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
  return(r);
}

int dbCursDefineSql_va(DBcurs *curs,char *sqlCmd,...)
{
  va_list    args;
  int    r;
  char    *valueName;
  char    *valueP;
  int    valueSize;
  int    valueType;

  r=dbCursDefineSqlCmd(curs,sqlCmd);
  if (r!=0)
  {
    dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
    return(r);
  };

  va_start(args,sqlCmd);
  while((valueName=va_arg(args,char *))!=NULL)
  {
    valueP   = va_arg(args,char *);
    valueSize= va_arg(args,int);
    valueType= va_arg(args,int);

    r=dbCursDefineValue(curs,valueName,valueP,valueSize,valueType);
    if (r!=0)
    {
      dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
      va_end(args);
      return(r);
    };
  };

  return(r);
}

int dbCursDefineSelect_va(DBcurs *curs,char *sqlCmd,...)
{
  va_list    args;
  int    r;

  int    n;
  char    *returnValueP;
/*
  char    returnValueSize;
  char    returnValueType;
*/
  int    returnValueSize;
  int    returnValueType;

  char    *valueName;
  char    *valueP;
  int    valueSize;
  int    valueType;

  r=dbCursDefineSqlCmd(curs,sqlCmd);
  if (r!=0)
  {
    dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
    return(r);
  };

  va_start(args,sqlCmd);
  n=0;
  while((returnValueP=va_arg(args,char *))!=NULL)
  {
    n++;
    returnValueSize= va_arg(args,int);
    returnValueType= va_arg(args,int);

    r=dbCursDefineRecvValue(curs,n,returnValueP,
            returnValueSize,
            returnValueType);
    if (r!=0)
    {
      dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
      va_end(args);
      return(r);
    };
  };

  while((valueName=va_arg(args,char *))!=NULL)
  {
    valueP   = va_arg(args,char *);
    valueSize= va_arg(args,int);
    valueType= va_arg(args,int);

    r=dbCursDefineValue(curs,valueName,valueP,valueSize,valueType);
    if (r!=0)
    {
      dbCursLogErrInfo(curs,sqlCmd,__FILE__,__LINE__);
      va_end(args);
      return(r);
    };
  };

  return(r);
}

