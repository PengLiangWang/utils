#include <stdio.h>
#include <signal.h>
#include "DBulog.h"
#include "businessdb.h"

int BusinessDataBaseOpen()
{
  char *getenv();
  char *UserID,*UserPWD,*DbName;
  int r;

  UserID = getenv(BUSINESS_USERNAME_ENV);
  if(UserID==NULL)
  {
    DBLOG("Envirnment %s missed\n",BUSINESS_USERNAME_ENV);
    return -1;
  }
  UserPWD = getenv(BUSINESS_USERPWD_ENV);
  if(UserPWD==NULL)
  {
    DBLOG("Envirnment %s missed",BUSINESS_USERPWD_ENV);
    return -1; 
  }
  DbName = getenv(BUSINESS_DATABASE_NAME);
  /*
  if(DbName==NULL)
  {
    DBlog_print(__FILE__,__LINE__,
      "Envirnment %s missed",BUSINESS_DATABASE_NAME);
    return(-1);
  */

  r = dbOpen(UserID,UserPWD,DbName);
  if(r!=0)
  {

    DBLOG("Database(%s) open error(%d)\n",UserID,r);
    return(r);
  }
  r = dbWaitLock(0);
  if(r!=0)
  {

    DBLOG("Database(%s) set wait mode error(%d)\n",UserID,r);
    return(r);
  }

  return(r);
}

int BusinessDataBaseClose()
{
/*
#! 为informix增加signal SIGCLD的设置，最后将被设置成IGN
*/
  int    r=0;
#ifdef ESQL_HIGH_VERSION
  signal(SIGCLD, SIG_DFL);
#endif
  r=dbClose();
#ifdef ESQL_HIGH_VERSION
  signal(SIGCLD, SIG_IGN);
#endif
   return r;
}
