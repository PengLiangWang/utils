//#include "ttserial.h"
#include "ttsys.h"
#include "ttdbserial.h"
#include <stdlib.h>

typedef struct 
{
   char   dbName[32];
   char   dbUser[32];
   char   dbPwsd[32];
   char   table[32];
   int    serverId;
   int    logMask;
   int    logLevel;
   char   logFile[32];
   char   logRemote[32];
   uint32 QQ;
   uint32 QA;
}SerialConfig;

static SerialConfig  config;

int GetSerailConfig(char *filename)
{
   int        res;
   char      *val;
   TConfig   *serialConf;
   char       file[256];

   memset(&config,0,sizeof(config));

   snprintf(file,256,"%s/etc/%s",(char *)getenv("HOME"),filename);
   res = ConfigLoad(file,&serialConf);
   if ( res )
   {
      ELOG(ERROR,"装载配置文件(%s)失败(%s:%d)",file,GetStrError(res),res);
      return res;
   }

   res = ConfigGetProperty(serialConf,"ttserial","dbname",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:dbname)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.dbName,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","dbuser",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:dbuser)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.dbUser,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","dbpwsd",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:dbpwsd)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.dbPwsd,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","table",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:table)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.table,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","serverId",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:serverId)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   config.serverId = atoi(val);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","logmask",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:logmask)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   config.logMask = atoi(val);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","loglevel",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:loglevel)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   if ( strncmp(val,"DEBUG",5) == 0 )
   {
      config.logLevel = LOG_LEVEL_DEBUG;
   }
   else if ( strncmp(val,"WARN",4) == 0 )
   {
      config.logLevel = LOG_LEVEL_WARN;
   }
   else
   {
      config.logLevel = LOG_LEVEL_INFO;
   }
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","logfile",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:logfile)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.logFile,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","logsvr",&val);
   if ( res )
   {
      ELOG(ERROR,"查找配置文件(%s)(ttserial:logsvr)失败(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   if ( strncmp(val,"NULL",4) == 0 )
   {
      config.logMask = LOG_FILE_MASK;
   }
   else
   {
     strncpy(config.logRemote,val,31);
     config.logMask |= LOG_HOST_MASK;
   }
   ConfigFree(&val);
   ConfigClose(&serialConf);

   snprintf(file,256,"%s/etc",(char *)getenv("HOME"));

   config.QQ = ftok(file,config.serverId);
   config.QA = ftok(file,config.serverId+1);
   if ( config.QQ < 0 || config.QA < 0 )
   {
      ELOG(ERROR,"文件系统(ftok)出现故障(%s:%d)", GetStrError(errno), errno);
      return errno;
   }

   return 0;
}

#define  SERIAL_CONFIG_FILE   "ttserial.ini"

int SerialInitialize(char *table,char *name,long maxSerialNo)
{
   int         res;
   MySerial    serial ;

   memset (&serial,0,sizeof(serial));
  
   strncpy(serial.serialName,name,32);
   serial.serialNo = 1;
   serial.maxSerialNo = maxSerialNo;

   res = dbBeginWork();
   if ( res )
   {
      goto E;
   }

   res = DB_MySerial_add(table,&serial);
   if ( res == 0 )
   {
      res = dbCommit();
   }

E:
   if ( res )
   {
      dbRollback();
   }

   return res;
}


void usage()
{
   fprintf(stderr,"ttserial tools version 1.0, dated 18 March 2011\n");
   
   fprintf(stderr,"Usage: ttserial [-i max serial no ] [-g] [-f config] [-n serial name]\n");
   fprintf(stderr,"  -i  init serial no (max serial no)\n");
   fprintf(stderr,"  -g  get serial no\n");
   fprintf(stderr,"  -f  config file no\n");
   fprintf(stderr,"  -n  serial name no\n");
   fprintf(stderr,"\n");
   exit(-1);
}

int main(int argc,char *argv[])
{
   int            i = 0,res,action = 0;
   char          *cfgFile = NULL,*serinalName = NULL;
   extern char   *optarg;
   long           maxSerialNo = -1;
   char           serialNo[7];

   while ( ( i = getopt(argc, argv, "i:f:n:g") ) != EOF )
   {
      switch (i)
      {
         case 'i':
            action = 1;
            maxSerialNo = atol(optarg);
            break;
         case 'g':
            action = 2;
            break;
         case 'f':
            cfgFile = optarg;
            break;
         case 'n':
            serinalName = optarg;
            break;
         default:
            usage();
      }
   }

   if ( cfgFile == NULL )
   {
      cfgFile = SERIAL_CONFIG_FILE;
   }

   if ( serinalName == NULL || !action )
   {
      usage();
   }
  
   if ( maxSerialNo < 0 )
   {
      maxSerialNo = 999999;
   }

   res = GetSerailConfig(cfgFile);
   if ( res )
   {
      ELOG(ERROR,"装载配置文件(%s)失败,服务退出",cfgFile);
      return -1;
   }

   memset(serialNo,0,sizeof(serialNo));

   eloginit(config.logLevel,"ttserial",NULL,config.logFile,config.logMask,config.logRemote );

   if ( action == 1 )
   {
      res = dbOpen(config.dbUser,config.dbPwsd,config.dbName);
      if ( res )
      {
         fprintf(stderr,"打开数据库(%s:%s)失败,错误码:%d", config.dbName, config.dbUser, res);
         return res;
      }

      res = SerialInitialize(config.table, serinalName, maxSerialNo);
      if ( res )
      {
         dbClose();
         return res;
      }

      dbClose();

      return res;
   }
   else if ( action == 2 )
   {
      res = GetSerialNo(config.serverId, serinalName, serialNo, 6);
      if ( res )
      {
         fprintf(stderr,"GetSerialNo Fail (%s:%d)\n",GetStrError(res),res);
      }
      else
      {
         fprintf(stdout,"GetSerialNo Success. %s=%s\n",serinalName, serialNo);
      }
      return res;
   }

   return 0;
}



