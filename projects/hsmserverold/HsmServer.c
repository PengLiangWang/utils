#include "ttsys.h"
#include "HsmCommand.h"
#include "HsmRes.h"
#include "HsmConfig.h"

static TSocket *GServerSocket = NULL;

#define MAX_CHILDS        256
#define MAX_WORK_LIMITS   128 * 128

static pid_t  GWorkersInfo[MAX_CHILDS];

static THsmServerConfig   GServerConfig;

static TMemPool          *GMemPool;

static int                GHsmServerFlag = 0;

int HsmCreateWorkers()
{
  int         i = 0;
  pid_t       pid;
  int         rv;

  for (i = 0; i < 256 ; i++ )
  {
    GWorkersInfo[i] = -1;
  }

  for (i = 0;i < GServerConfig.minWorker  ;++i )
  {
    pid = fork();
    if (pid < 0)
    {
      ELOG(ERROR,"创建加密处理进程失败,错误码:%d,错误信息:%s",errno,GetStrError(errno));
      return errno;
    }
    if (pid == 0)
    {
      rv = HsmWorker(MAX_WORK_LIMITS,GServerSocket);
      exit(0);
    }
    if (pid > 0)
    {
      GWorkersInfo[i] = pid;
      usleep(100000);
    }
  }
  return 0;
}


void HsmDaemonCleanup()
{
  int i;
  
  sigset(SIGCLD, SIG_IGN);

  for (i = 0 ; i < MAX_CHILDS;i++ )
  {
    if (GWorkersInfo[i] > 0)
    {
      kill(GWorkersInfo[i],SIGUSR2);
      GWorkersInfo[i] = -1;
      usleep(100000);
    }
  }

  HsmResDestory();
  
  if ( GServerSocket != NULL )
  {
    SocketClose(&GServerSocket);
  }

  return ;
}


void HsmWorkerRestart(int sig)
{
  int       i = 0,index,wstat;
  int       rv;
  pid_t     pid;

  sigset(SIGCLD, SIG_IGN);

  while ((pid = waitpid(-1,&wstat,WNOHANG)) > 0)
  {
    index = -1;
    for (i = 0; i < MAX_CHILDS;i++ )
    {
      if (GWorkersInfo[i] == pid)
      {
        GWorkersInfo[i] = -1;
        index = i;
        break;
      }
    }
    if (index == -1)
    {
      break;
    }
    pid = fork();
    if ( pid < 0 )
    {
      HsmDaemonCleanup();
      exit(0);
    }
    if ( pid == 0 )
    {
      rv = HsmWorker(MAX_WORK_LIMITS,GServerSocket);
      exit(rv);
    }
    else if ( pid > 0 )
    {
      GWorkersInfo[index] = pid;
      usleep(100000);
      break;
    }
  }

  sigset(SIGCLD, HsmWorkerRestart);

  return ;
}


void HsmServerStop( int sig)
{
  char   pidFile[256];

  sigset(SIGCLD, SIG_IGN);
  HsmDaemonCleanup();
  GHsmServerFlag = 1;
  
  memset(pidFile,0,sizeof(pidFile));
  sprintf(pidFile,"%s/etc/.HsmServer",(char *)getenv("HOME"));
  unlink(pidFile);
  return ;
}

int main(int argc,char *argv[])
{
  int        rv ;
  char       pidFile[256];
  int        i,count;
  pid_t      pid;

  memset(pidFile,0,sizeof(pidFile));
  sprintf(pidFile,"%s/etc/.HsmServer",(char *)getenv("HOME"));

  if ( access(pidFile,0 ) == 0 )
  {
    if ( argc == 2 && strcmp(argv[1],"stop") == 0 )
    {
      char   shell[256];
      memset(shell,0,sizeof(shell));
      sprintf(shell,"cat %s|awk '{print \"kill\",$0}'|sh",pidFile);
      system(shell);
      exit(0);
    }
    else
    {
      printf("\n加密服务已启动,请查看.\n\n");
      return 0;
    }
  }

  if ( argc != 2 || strcmp(argv[1],"start") == 0 )
  {
    if ( fork() )
    {
      exit(0);
    }
  }

  memset(&GServerConfig,0,sizeof(GServerConfig));
  rv = HsmLoadConfig(HSM_SERVER_CONF_FILE,&GServerConfig);
  if ( rv )
  {
    fprintf(stderr,"HsmServer: 启动失败,装载配置文件[%s]失败\n",HSM_SERVER_CONF_FILE);
    fflush(stderr);
    return rv;
  }

  eloginit(GServerConfig.level,"HSM",NULL,GServerConfig.logFile, LOG_FILE_MASK, NULL );

  rv = HsmResCreate(GServerConfig.mutexId,GServerConfig.shmemId,GServerConfig.msgId);
  if ( rv )
  {
    fprintf(stderr,"HsmServer: 启动失败,创建加密系统资源失败\n");
    fflush(stderr);
    return rv;
  }

  rv = MemPoolCreateEx(&GMemPool,NULL);
  if ( rv )
  {
    fprintf(stderr,"HsmServer: 启动失败,创建加密系统内存池失败\n");
    fflush(stderr);
    HsmResDestory();
    return rv;
  }

  HsmCommandInit();

  rv = HsmLoadCommand(HSM_SERVER_CONF_FILE,&count,GMemPool);
  if ( rv )
  {
    fprintf(stderr,"HsmServer: 启动失败,装载加密系统指令失败\n");
    fflush(stderr);
    HsmResDestory();
    MemPoolDestoryEx(GMemPool);
    return rv;
  }

  HsmKeyBoxSetDatabase(GServerConfig.dbName,GServerConfig.dbUser,GServerConfig.dbPswd);

  rv = HsmKeyBoxLoadKey();
  if ( rv )
  {
    fprintf(stderr,"HsmServer: 启动失败,装载加密系统密钥数据失败\n");
    fflush(stderr);
    HsmResDestory();
    MemPoolDestoryEx(GMemPool);
    return rv;
  }

  rv = SocketServer(&GServerSocket,NULL,GServerConfig.listenPort);
  if ( rv )
  {
    printf("HsmServer: 启动失败,创建加密系统网络服务\n          错误码:%d,错误信息:%s\n",rv,GetStrError(rv));
    HsmResDestory();
    MemPoolDestoryEx(GMemPool);
    return rv;
  }

  setsid();
  signal( SIGINT,  SIG_IGN );
  signal( SIGPIPE, SIG_IGN );
  signal( SIGQUIT, SIG_IGN );
  signal( SIGHUP,  SIG_IGN );
  signal(SIGCLD, SIG_IGN);
  sigset( SIGTERM, HsmServerStop );

  rv = HsmCreateWorkers();
  if ( rv )
  {
    HsmDaemonCleanup();
    return rv;
  }

  /* 记录启动PID */ 
  {
    FILE *fp = fopen(pidFile,"w");
    if ( fp )
    {
      fprintf(fp,"%d",getpid());
      fclose(fp);
    }
  }

  sigset( SIGCLD,  HsmWorkerRestart );

  for (; ; )
  {
    if ( GHsmServerFlag )
    {
      break;
    }

    usleep(100000);
  }
  return 0;
}

