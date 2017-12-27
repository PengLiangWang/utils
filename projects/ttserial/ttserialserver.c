
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "ttsys.h"
#include "ttdbserial.h"

extern int errno;

typedef struct
{
   long     mtype;
   char     name[33];
   long     serialNo;
   int      batchNo;
}SerialControl;

#define SERIAL_STEP            500

static TMessageQueue   QueueQ,QueueA;

typedef struct 
{
  char      name[33];
  long      serialNo;
  long      maxSerialNo;
}SNrecord;

#define MAXSNCNT     12

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


static SNrecord      sn[MAXSNCNT];
static SerialControl scm;
static int           sncnt = 0;
static SerialConfig  config;

void ServerDataInit()
{
   memset(&sn,0,sizeof(SNrecord) * MAXSNCNT);
   memset(&scm,0,sizeof(SerialControl));
   memset(&config,0,sizeof(config));
   return ;
}


void ServerStop(int sig)
{
   ELOG(INFO,"��ˮ��������˳�");
   MessageQueueDestory(config.QQ);
   MessageQueueDestory(config.QA);
   exit(0) ;
}

static void ServerDeamon()
{
   if(fork() != 0)
   {
      exit(0);
   }

   setsid();
   sigset ( SIGTERM , ServerStop );
   signal ( SIGINT  , SIG_IGN );
   signal ( SIGQUIT , SIG_IGN );
   signal ( SIGHUP  , SIG_IGN );
   signal ( SIGPIPE , SIG_IGN );
   signal ( SIGCLD  , SIG_IGN );

   return  ;
}

int UpdateSerialNo(int index)
{
   MySerial       ser;
   Lock_Info      sInfo;
   int            res;
   long           a;

   res = dbBeginWork();
   if ( res )
   {
      ELOG(ERROR,"���ݿ⿪ʼ����(dbBeginWork)ʧ��,���ݿ������:%d",res);
      return res;
   }
   
   ELOG(INFO,"�����ݿ��л�ȡ(%s)�µ���ˮ��",scm.name);

   res = DB_MySerial_read_lock_by_name(config.table,scm.name,&ser,&sInfo);
   if ( res == 0 )
   {
      if ( ser.serialNo >= ser.maxSerialNo )
      {
         ser.serialNo = 1;
      }

      sn[index].serialNo = ser.serialNo;
      
      a = ser.serialNo + SERIAL_STEP;
      if ( a >= ser.maxSerialNo)
      {
         ser.serialNo = 1;
         sn[index].maxSerialNo = ser.maxSerialNo;
      }
      else 
      {
         ser.serialNo = a;
         sn[index].maxSerialNo = a;
      }
      scm.serialNo = sn[index].serialNo++;
      res = DB_MySerial_update_by_name(config.table,scm.name,&ser);
      DB_MySerial_free_lock(&sInfo);
   }

   if ( res == 0 )
   {
      res = dbCommit();
      if ( res )
      {
         dbRollback();
      }
   }
   else
   {
      dbRollback();
   }
   
   if ( res )
   {
      ELOG(ERROR,"������ˮ��ʧ��,���ݿ������:%d",res);
      scm.serialNo = -1;
   }

   return res;
}

int GetSerialNo(int index)
{
  long           serialNo;
  int            res;
  
  if ( scm.batchNo < 0 )
  {
    scm.batchNo = 1;
  }

  if ( sn[index].serialNo > 0 )
  {
    serialNo = sn[index].serialNo + scm.batchNo;
    if (serialNo < sn[index].maxSerialNo)
    {
      scm.serialNo = sn[index].serialNo;
      sn[index].serialNo = serialNo;
      return 0;
    }
  }

  res = dbOpen(config.dbUser,config.dbPwsd,config.dbName);
  if ( res )
  {
     ELOG(ERROR,"�����ݿ�(%s:%s)ʧ��,������:%d", config.dbName, config.dbUser, res);
     return res;
  }
  
  res = UpdateSerialNo(index);
  if ( res )
  {
     ELOG(ERROR,"��ȡ�������ˮʧ��(%d)", res);
     dbClose();
     return res;
  }

  dbClose();

  return 0;
}


int GetSerailConfig(char *filename)
{
   int        res;
   char      *val;
   TConfig   *serialConf;
   char       file[256];

   snprintf(file,256,"%s/etc/%s",(char *)getenv("HOME"),filename);
   res = ConfigLoad(file,&serialConf);
   if ( res )
   {
      ELOG(ERROR,"װ�������ļ�(%s)ʧ��(%s:%d)",file,GetStrError(res),res);
      return res;
   }

   res = ConfigGetProperty(serialConf,"ttserial","dbname",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:dbname)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.dbName,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","dbuser",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:dbuser)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.dbUser,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","dbpwsd",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:dbpwsd)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.dbPwsd,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","table",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:table)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.table,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","serverId",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:serverId)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   config.serverId = atoi(val);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","logmask",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:logmask)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   config.logMask = atoi(val);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","loglevel",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:loglevel)ʧ��(%s:%d)", file, GetStrError(res), res);
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
      ELOG(ERROR,"���������ļ�(%s)(ttserial:logfile)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&serialConf);
      return res;
   }
   strncpy(config.logFile,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(serialConf,"ttserial","logsvr",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(ttserial:logsvr)ʧ��(%s:%d)", file, GetStrError(res), res);
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
      ELOG(ERROR,"�ļ�ϵͳ(ftok)���ֹ���(%s:%d)", GetStrError(errno), errno);
      return errno;
   }

   return 0;
}

void usage()
{
   fprintf(stderr,"\n");
   fprintf(stderr,"Usage: ttserial -f conf_file -d -h \n");
   fprintf(stderr,"                -f conf_file   config file\n");
   fprintf(stderr,"                -d             is daemon\n");
   fprintf(stderr,"                -h             help\n");
   fprintf(stderr,"\n");
   exit(-1);
}

#define  SERIAL_CONFIG_FILE   "ttserial.ini"

int main(int argc,char **argv)
{
   int            i = 0;
   int            res ;
   int            daemon = 0;
   char          *cfgFile = NULL;
   extern char   *optarg;

   while ( ( i = getopt(argc, argv, "df:h") ) != EOF )
   {
      switch (i)
      {
         case 'd':
            daemon = 1;
            break;
         case 'f':
            cfgFile = optarg;
            break;
         default:
            usage();
      }
   }

   if ( cfgFile == NULL )
   {
      cfgFile = SERIAL_CONFIG_FILE;
   }

   ServerDataInit();

   res = GetSerailConfig(cfgFile);
   if ( res )
   {
      ELOG(ERROR,"װ�������ļ�(%s)ʧ��,�����˳�",cfgFile);
      return -1;
   }

   if ( daemon )
   {
      ServerDeamon();
   }
   else
   {
      sigset ( SIGTERM , ServerStop );
      signal ( SIGINT  , SIG_IGN );
      signal ( SIGQUIT , SIG_IGN );
      signal ( SIGHUP  , SIG_IGN );
      signal ( SIGPIPE , SIG_IGN );
      signal ( SIGCLD  , SIG_IGN );
   }

   eloginit(config.logLevel,"ttseriald",NULL,config.logFile,config.logMask,config.logRemote);

   memset(&QueueQ,0,sizeof(QueueQ));
   memset(&QueueA,0,sizeof(QueueA));

   res = MessageQueueCreate(&QueueQ,config.QQ);
   if ( res )
   {
      ELOG(ERROR,"��������(%u)ʧ��(%s:%d)",config.QQ,GetStrError(res),res);
      return res;
   }

   res = MessageQueueCreate(&QueueA,config.QA);
   if ( res )
   {
      ELOG(ERROR,"��������(%u)ʧ��(%s:%d)",config.QA,GetStrError(res),res);
      MessageQueueDestory(config.QQ);
      return res;
   }

   

   ELOG(DEBUG,"��ˮ��������Ѿ�����,��ʼ������");
   for (; ; )
   {
      memset(&scm,0,sizeof(SerialControl));
      scm.mtype = 0;

      res = MessageQueueRecv(&QueueQ,(TMessageQueueBuffer *)&scm,sizeof(scm) - sizeof(long));
      if ( res )
      {
         ELOG(ERROR,"������ˮ��������ʧ��(%s:%d)",GetStrError(res),res);
         break;
      }
      sighold(SIGTERM);
      i = 0;
      while ( (i < sncnt) && (strcmp(sn[i].name,scm.name) != 0) )
      {
         ++i;
      }
      if(i == sncnt)
      {
         if( i >= MAXSNCNT )
         {
            scm.serialNo = -1;
            scm.batchNo = -1;
            ELOG(ERROR,"ϵͳû���㹻����ˮ������(%s)",scm.name);
         }
         else
         {
            ++sncnt;
            strcpy(sn[i].name,scm.name);
            sn[i].serialNo = -1;
            sn[i].maxSerialNo = -1;
            res = GetSerialNo(i);
         }
      }
      else
      {
         res = GetSerialNo(i);
      }

      if ( res )
      {
         ELOG(ERROR,"������ˮʧ�� %d",res);
      }

       res = MessageQueueSend(&QueueA,(TMessageQueueBuffer *)&scm,sizeof(scm) - sizeof(long));
       if ( res )
       {
         ELOG(ERROR,"������ˮ�������ʧ��(%s:%d)",GetStrError(res),res);
         sigrelse(SIGTERM);
         break;
       }
       sigrelse(SIGTERM);
   }

   return 0;
}




