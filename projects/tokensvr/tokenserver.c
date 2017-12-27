
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "ttsys.h"
#include "etoken.h"

extern int errno;


static TMessageQueue  *QueueQ = NULL , *QueueA = NULL;

typedef struct 
{
   int    serverId;
   int    tokenMax;
   int    logMask;
   int    logLevel;
   char   logFile[32];
   char   logRemote[32];
}TokenConfig;


static TTokenMsg     tokenMsg;
static TokenConfig   config;

void ServerDataInit()
{
   memset(&tokenMsg,0,sizeof(TTokenMsg));
   memset(&config,0,sizeof(config));
   return ;
}


void ServerStop(int sig)
{
   ELOG(INFO,"token server success exit.");
   ToketQueueClose(config.serverId);
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



int GetTokenConfig(char *filename)
{
   int        res;
   char      *val;
   TConfig   *tokenConf;
   char       file[256];

   snprintf(file,256,"%s/etc/%s",(char *)getenv("HOME"),filename);
   res = ConfigLoad(file,&tokenConf);
   if ( res )
   {
      ELOG(ERROR,"װ�������ļ�(%s)ʧ��(%s:%d)",file,GetStrError(res),res);
      return res;
   }

   res = ConfigGetProperty(tokenConf,"token","serverId",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(token:serverId)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&tokenConf);
      return res;
   }
   config.serverId = atoi(val);
   ConfigFree(&val);

   res = ConfigGetProperty(tokenConf,"token","maxId",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(token:maxId)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&tokenConf);
      return res;
   }
   config.tokenMax = atoi(val);
   ConfigFree(&val);

   res = ConfigGetProperty(tokenConf,"token","logmask",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(token:logmask)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&tokenConf);
      return res;
   }
   config.logMask = atoi(val);
   ConfigFree(&val);

   res = ConfigGetProperty(tokenConf,"token","loglevel",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(token:loglevel)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&tokenConf);
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

   res = ConfigGetProperty(tokenConf,"token","logfile",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(token:logfile)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&tokenConf);
      return res;
   }
   strncpy(config.logFile,val,31);
   ConfigFree(&val);

   res = ConfigGetProperty(tokenConf,"token","logsvr",&val);
   if ( res )
   {
      ELOG(ERROR,"���������ļ�(%s)(token:logsvr)ʧ��(%s:%d)", file, GetStrError(res), res);
      ConfigClose(&tokenConf);
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
   ConfigClose(&tokenConf);

   return 0;
}

void usage()
{
   fprintf(stderr,"\n");
   fprintf(stderr,"Usage: tokensvr -f conf_file -d -h \n");
   fprintf(stderr,"                -f conf_file   config file\n");
   fprintf(stderr,"                -d             is daemon\n");
   fprintf(stderr,"                -h             help\n");
   fprintf(stderr,"\n");
   exit(-1);
}

#define  TOKEN_CONFIG_FILE   "token.ini"

int main(int argc,char **argv)
{
   int            i = 0;
   int            res ;
   int            daemon = 0;
   char          *cfgFile = NULL;
   extern char   *optarg;
   int            maxTrans = 0;
   int            currTrans = 0;

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
      cfgFile = TOKEN_CONFIG_FILE;
   }

   ServerDataInit();

   res = GetTokenConfig(cfgFile);
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

   eloginit(config.logLevel,"tokend",NULL,config.logFile,config.logMask,config.logRemote);

   res = TokenQueueCreate(config.serverId, &QueueQ, &QueueA);
   if ( res )
   {
      ELOG(ERROR,"�������ƶ���ʧ��", GetStrError(res),res);
      return res;
   }

   ELOG(DEBUG,"���Ʒ����������Ѿ�����,��ʼ������");
   
   maxTrans = config.tokenMax;

   for (; ; )
   {
      memset(&tokenMsg,0,sizeof(TTokenMsg));
      tokenMsg.mtype = 0;

      res = MessageQueueRecv(QueueQ,(TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long));
      if ( res )
      {
         ELOG(ERROR,"�������Ʒ�������ʧ��(%s:%d)",GetStrError(res),res);
         break;
      }
      sighold(SIGTERM);
      
      switch (tokenMsg.rtype)
      {
         case ETOKEN_GET_STATUS:
            tokenMsg.tcode = ETOKEN_ERR_SUCCESS;
            tokenMsg.tnum = currTrans;
            tokenMsg.tmax = maxTrans;
            break;
         case ETOKEN_INCREASE_MAX:
            maxTrans++;
            tokenMsg.tcode = ETOKEN_ERR_SUCCESS;
            tokenMsg.tnum = currTrans;
            tokenMsg.tmax = maxTrans;
            break;
         case ETOKEN_DECREASE_MAX:
            if ( maxTrans <= 0)
            {
               ELOG(ERROR, "���Ƶ����ֵ�Ƿ�(maxId = %d)", maxTrans);
               tokenMsg.tcode = ETOKNE_ERR_DEFAULT;
            }
            else
            {
               maxTrans--;
               tokenMsg.tcode = ETOKEN_ERR_SUCCESS;
            }
            tokenMsg.tnum = currTrans;
            tokenMsg.tmax = maxTrans;
            break;
         case ETOKEN_INCREASE_CURRENT:
            if ( currTrans >= maxTrans )
            {
               ELOG(ERROR, "%d�������Ѿ���ȡ��", maxTrans);
               tokenMsg.tcode = ETOKEN_ERR_FULL;
            }
            else
            {
               currTrans++;
               tokenMsg.tcode = ETOKEN_ERR_SUCCESS;
            }
            tokenMsg.tnum = currTrans;
            tokenMsg.tmax = maxTrans;
            break;
         case ETOKEN_DECREASE_CURRENT:
            if ( currTrans <= 0 )
            {
               ELOG(ERROR, "���κ����ƿɹ��ͷ�(maxId = %d)", maxTrans);
               tokenMsg.tcode = ETOKNE_ERR_DEFAULT;
            }
            else
            {
               currTrans--;
               tokenMsg.tcode = ETOKEN_ERR_SUCCESS;
            }
            tokenMsg.tnum = currTrans;
            tokenMsg.tmax = maxTrans;
            break;
         default:
            ELOG(ERROR,"��֧�ֵ�����ָ��(%d)", tokenMsg.rtype);
            tokenMsg.tcode = ETOKNE_ERR_REQTYPE;
            tokenMsg.tnum = currTrans;
            tokenMsg.tmax = maxTrans;
            break;
      }

       res = MessageQueueSend(QueueA,(TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long));
       if ( res )
       {
         ELOG(ERROR,"������������ʧ��(%s:%d)",GetStrError(res),res);
         sigrelse(SIGTERM);
         break;
       }
       sigrelse(SIGTERM);
   }

   return 0;
}




