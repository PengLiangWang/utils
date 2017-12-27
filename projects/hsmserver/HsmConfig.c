#include "ttsys.h"
#include "HsmConfig.h"
#include "HsmCommand.h"
#include "HsmModule.h"

static int HsmCopyConfig(TConfig *config,THsmServerConfig *confData)
{
  int        rv ;
  char      *cfgval;

  rv = ConfigGetProperty(config,"HsmParams","ListenPort",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[HsmParams<ListenPort>]失败,错误码:%d",rv);
    return rv;
  }
  
  confData->listenPort = strtol(cfgval,NULL,10);

  ConfigFree(&cfgval);

  if ( confData->listenPort < 10 )
  {
    ELOG(ERROR,"配置信息[HsmParams<ListenPort>]非法");
    return TTS_EINVAL;
  }

  rv = ConfigGetProperty(config,"HsmParams","MutexId",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[HsmParams<MutexId>]失败,错误码:%d",rv);
    return rv;
  }
  
  confData->mutexId = strtol(cfgval,NULL,16);

  ConfigFree(&cfgval);

  if ( confData->mutexId < 0 )
  {
    ELOG(ERROR,"配置信息[HsmParams<MutexId>]非法");
    return TTS_EINVAL;
  }

  rv = ConfigGetProperty(config,"HsmParams","ShmemId",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","HsmParams","ShmemId",rv);
    return rv;
  }
  
  confData->shmemId = strtol(cfgval,NULL,16);

  ConfigFree(&cfgval);

  if ( confData->shmemId < 0 )
  {
    ELOG(ERROR,"配置信息[%s<%s>]非法","HsmParams","ShmemId");
    return TTS_EINVAL;
  }

  rv = ConfigGetProperty(config,"HsmParams","MsgId",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","HsmParams","MsgId",rv);
    return rv;
  }
  
  confData->msgId = strtol(cfgval,NULL,16);

  ConfigFree(&cfgval);

  if ( confData->msgId < 0 )
  {
    ELOG(ERROR,"配置信息[%s<%s>]非法","HsmParams","msgId");
    return TTS_EINVAL;
  }

  rv = ConfigGetProperty(config,"HsmParams","MaxWorkers",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","HsmParams","MaxWorkers",rv);
    return rv;
  }
  
  confData->maxWorker = strtol(cfgval,NULL,10);

  ConfigFree(&cfgval);

  if ( confData->maxWorker < 0 )
  {
    ELOG(ERROR,"配置信息[%s<%s>]非法","HsmParams","MaxWorkers");
    return TTS_EINVAL;
  }

  rv = ConfigGetProperty(config,"HsmParams","MinWorkers",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","HsmParams","MinWorkers",rv);
    return rv;
  }
  
  confData->minWorker = strtol(cfgval,NULL,10);

  ConfigFree(&cfgval);

  if ( confData->minWorker < 0 || confData->minWorker > confData->maxWorker )
  {
    ELOG(ERROR,"配置信息[%s<%s>]非法","HsmParams","MinWorkers");
    return TTS_EINVAL;
  }

  rv = ConfigGetProperty(config,"Log","level",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","Log","level",rv);
    return rv;
  }
  if ( strcmp(cfgval, "debug") == 0 )
  {
     confData->level = LOG_LEVEL_DEBUG;
  }
  else if ( strcmp(cfgval, "info") == 0 )
  {
     confData->level = LOG_LEVEL_INFO;
  }
  else
  {
     confData->level = LOG_LEVEL_WARN;
  }

  rv = ConfigGetProperty(config,"Log","file",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","Log","file",rv);
    return rv;
  }
  strncpy(confData->logFile,cfgval,32);
  ConfigFree(&cfgval);

  rv = ConfigGetProperty(config,"HsmParams","DatabaseUser",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","HsmParams","DatabaseUser",rv);
    return rv;
  }
  strncpy(confData->dbUser,cfgval,32);
  ConfigFree(&cfgval);

  rv = ConfigGetProperty(config,"HsmParams","DatabaseName",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","HsmParams","DatabaseName",rv);
    return rv;
  }
  strncpy(confData->dbName,cfgval,32);
  ConfigFree(&cfgval);

  rv = ConfigGetProperty(config,"HsmParams","DatabasePwsd",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d","HsmParams","DatabasePwsd",rv);
    return rv;
  }
  strncpy(confData->dbPswd,cfgval,32);
  ConfigFree(&cfgval);

  return TTS_SUCCESS;
}

int HsmLoadConfig(char *fileName,THsmServerConfig *confData)
{
  int        rv ;
  TConfig   *config;
  char       filePath[256];

  sprintf(filePath,"%s/etc/%s",(char *)getenv("HOME"),fileName);

  rv = ConfigLoad((const char *)filePath,&config);
  if ( rv )
  {
    ELOG(ERROR,"装载配置文件<%s>失败,错误码:%d,错误信息:%s",fileName,rv,GetStrError(rv));
    return rv;
  }

  rv = HsmCopyConfig(config,confData);
  if ( rv )
  {
    ELOG(ERROR,"装载配置文件<%s>失败,错误码:%d,错误信息:%s",fileName,rv,GetStrError(rv));
    ConfigClose(&config);
    return rv;
  }
  
  ConfigClose(&config);

  return rv;

}

struct _command_file_
{
  char    modName[33];
  char    filePath[256];
};

typedef struct _command_file_ TCommandFile;

int HsmCopyCommand(TConfig *config,int cmdIdx,TCommandFile *cmdConfig)
{
  int        rv ;
  char      *cfgval;
  char       section[33];
  char       fileName[256];

  memset(section,0,sizeof(section));
  sprintf(section,"HsmCmdFile%02d",cmdIdx);

  rv = ConfigGetProperty(config,section,"ModuleName",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d",section,"modName",rv);
    return rv;
  }
  strncpy(cmdConfig->modName,cfgval,32);
  ConfigFree(&cfgval);

  rv = ConfigGetProperty(config,section,"FilePath",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[%s<%s>]失败,错误码:%d",section,"sofile",rv);
    return rv;
  }
  strncpy(cmdConfig->filePath,cfgval,255);
  ConfigFree(&cfgval);
  memset(fileName,0,sizeof(fileName));
  if ( strncmp(cmdConfig->filePath,"$HOME/",6) == 0 )
  {
    sprintf(fileName,"%s/%s",(char *)getenv("HOME"),cmdConfig->filePath + 6);
    memset(cmdConfig->filePath,0,sizeof(cmdConfig->filePath));
    strncpy(cmdConfig->filePath,fileName,255);
  }

  return TTS_SUCCESS;
}

int HsmLoadCommand(char *fileName,int *count,TMemPool *MemPool)
{
  int          rv ;
  TConfig     *config;
  char         filePath[256];
  char        *cfgval;
  int          cmdFiles,i;
  TCommandFile cmdFile;
  

  sprintf(filePath,"%s/etc/%s",(char *)getenv("HOME"),fileName);

  rv = ConfigLoad((const char *)filePath,&config);
  if ( rv )
  {
    ELOG(ERROR,"装载配置文件<%s>失败,错误码:%d,错误信息:%s",fileName,rv,GetStrError(rv));
    return rv;
  }

  rv = ConfigGetProperty(config,"HsmCommand","cmdFiles",&cfgval);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"取配置信息[HsmCommand<%s>]失败,错误码:%d","cmdFiles",rv);
    ConfigClose(&config);
    return rv;
  }
  cmdFiles = strtol(cfgval,NULL,10);
  ConfigFree(&cfgval);

  if ( cmdFiles <= 0 || cmdFiles > 256 )
  {
    ELOG(ERROR,"命令配置参数[HsmCommand<CmdFiles>]太大");
    ConfigClose(&config);
    return TTS_EINVAL;
  }


  for ( i = 0; i < cmdFiles; i++)
  {
    memset(&cmdFile,0,sizeof(cmdFile));

    rv = HsmCopyCommand(config,i+1,&cmdFile);
    if ( rv != TTS_SUCCESS )
    {
      ConfigClose(&config);
      return rv;
    }
    rv = HsmLoadModule(&cmdFile,MemPool);
    if ( rv != TTS_SUCCESS )
    {
      ConfigClose(&config);
      return rv;
    }
  }

  ConfigClose(&config);

  if ( count )
  {
    *count = cmdFiles;
  }
  
  return rv;
}


int HsmLoadModule(TCommandFile *cmdFile,TMemPool *pool)
{
  int                rv;
  TDllHandle        *dll;
  TDllHandleSym      module;
  THsmModule        *hsmModule;
  THsmCommandTable  *cmdTable;
  int                i;

  rv = DllLoad(&dll,(const char*)cmdFile->filePath,pool);
  if ( rv != TTS_SUCCESS )
  {
    char  errBuf[128];
    memset(errBuf,0,sizeof(errBuf));
    ELOG(ERROR,"装载DLL<%s>文件失败,错误码:%d,错误信息:%s",cmdFile->filePath,rv,DllError(dll,errBuf,sizeof(errBuf)-1));
    return rv;
  }

  rv = DllSym(&module, dll, (const char *)cmdFile->modName);
  if ( rv != TTS_SUCCESS )
  {
    char  errBuf[128];
    memset(errBuf,0,sizeof(errBuf));
    ELOG(ERROR,"装载Module<%s>失败,错误码:%d,错误信息:%s",cmdFile->modName,rv,DllError(dll,errBuf,sizeof(errBuf)-1));
    return rv;
  }
  hsmModule = ( THsmModule *)module;
  
  for ( i = 0; i < hsmModule->numOfCmd; i++)
  {
    cmdTable = &(hsmModule->table[i]);
    rv = HsmCommandAdd( cmdTable->command,cmdTable->success,cmdTable->cmdFunc );
    if ( rv != TTS_SUCCESS )
    {
      ELOG(ERROR,"增加FUNCTION<%s>失败,错误码:%d,错误信息:%s",cmdTable->command,rv,GetStrError(rv));
      return rv;
    }
  }

  return TTS_SUCCESS;
}




