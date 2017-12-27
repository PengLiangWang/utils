#include "ttsys.h"
#include "etoken.h"
#include <stdlib.h>

typedef struct 
{
   int    serverId;
   int    tokenMax;
   int    logMask;
   int    logLevel;
   char   logFile[32];
   char   logRemote[32];
}TokenConfig;

static TokenConfig  config;

int GetTokenConfig(char *filename)
{
   int        res;
   char      *val;
   TConfig   *tokenConf;
   char       file[256];

   memset(&config, 0, sizeof(config));

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

#define  TOKEN_CONFIG_FILE   "token.ini"


void usage()
{
   fprintf(stderr,"etoken tools version 1.0, dated 18 March 2011\n");
   
   fprintf(stderr,"Usage: etoken [-i increase ] [-d decrease] [-s get status] [-a get token] [-b recede] [-f config] \n");
   fprintf(stderr,"  -i  increase max\n");
   fprintf(stderr,"  -d  decrease max\n");
   fprintf(stderr,"  -s  get token server status\n");
   fprintf(stderr,"  -a  get token\n");
   fprintf(stderr,"  -b  recede token\n");
   fprintf(stderr,"\n");
   exit(-1);
}

int main(int argc,char *argv[])
{
   int            i = 0,res,action = 0;
   char          *cfgFile = NULL;
   extern char   *optarg;
   int            maxTrans,currTrans;

   while ( ( i = getopt(argc, argv, "idsabf:") ) != EOF )
   {
      switch (i)
      {
         case 'i':
            action = 1;
            break;
         case 'd':
            action = 2;
            break;
         case 's':
            action = 3;
            break;
         case 'a':
            action = 4;
            break;
         case 'b':
            action = 5;
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

   if ( !action )
   {
      usage();
   }
  
   res = GetTokenConfig(cfgFile);
   if ( res )
   {
      ELOG(ERROR,"װ�������ļ�(%s)ʧ��,�����˳�",cfgFile);
      return -1;
   }

   maxTrans = currTrans = -1;
   eloginit(config.logLevel,"etoken",NULL,config.logFile,config.logMask,config.logRemote );

   if ( action == 1 )
   {
      printf("������������:\n");
      res = TokenIncrease(config.serverId ,&maxTrans, &currTrans);
      printf("MAX TOKEN = %d\n", maxTrans);
      printf("USEING TOKEN = %d\n", currTrans);
      if ( res )
      {
         printf("token fail.\n");
      }
      return res;
   }
   else if ( action == 2 )
   {
      printf("������������:\n");
      res = TokenDecrease(config.serverId ,&maxTrans, &currTrans);
      printf("MAX TOKEN = %d\n", maxTrans);
      printf("USEING TOKEN = %d\n", currTrans);
      if ( res )
      {
         printf("token fail.\n");
      }
      return res;
   }
   else if ( action == 3 )
   {
      printf("����״��:\n");
      res = TokenInfo(config.serverId ,&maxTrans, &currTrans);
      printf("MAX TOKEN = %d\n", maxTrans);
      printf("USEING TOKEN = %d\n", currTrans);
      if ( res )
      {
         printf("token fail.\n");
      }
      return res;
   }
   else if ( action == 4 )
   {
      printf("��ȡ����:\n");
      res = TokenGet(config.serverId ,&maxTrans, &currTrans);
      printf("MAX TOKEN = %d\n", maxTrans);
      printf("USEING TOKEN = %d\n", currTrans);
      if ( res )
      {
         printf("token fail.\n");
      }
      return res;
   }
   else if ( action == 5 )
   {
      printf("�ͷ�����:\n");
      res = TokenRecede(config.serverId ,&maxTrans, &currTrans);
      printf("MAX TOKEN = %d\n", maxTrans);
      printf("USEING TOKEN = %d\n", currTrans);
      if ( res )
      {
         printf("token fail.\n");
      }
      return res;
   }

   return 0;
}



