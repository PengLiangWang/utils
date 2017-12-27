#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#include "logger.h"
#include "IBcom.h"
#include "IBnote.h"
#include "IBerr.h"
#include "IBsec.h"

extern int errno;
static int server_sock;

static char *getProgramName(char *msgType,char *transCode,char *transMode,char *group)
{
  static char program[32];
  int         rv;

  rv = IBGetTransProgram(msgType,transCode,transMode,program,group);
  if (rv != 0)
    return NULL;

  return program;
}

static int executeThisProgram(char *program)
{
  char    *exePath;

  exePath = (char*)GetExecFileName(program);

  execlp(exePath,program,NULL);

  logger(__FILE__,__LINE__,"execl %s ERROR %d,%s",exePath,errno,strerror(errno));

  return -1;
}

int IBserver()
{
  struct sockaddr_in  addr;
  int                 sock;
  int                 opt,r;
  char                local_node_id[11];
  char                addrStat[4];
  char                addr1[32];
  char                addr2[32];
  int                 portNo;


  r = IBsecGetSecNode(local_node_id,10);
  if (r != 0)
  {
    fprintf(stderr,"获取本地安全节点编码失败,请检查\n");
    return -1;
  }

  r = IBGetAddr(local_node_id,addrStat,addr1,addr2,&portNo);
  if (r != 0)
  {
    fprintf(stderr,"获取本地节点通讯参数失败,请检查\n");
    return -1;
  }

  sock = socket ( AF_INET, SOCK_STREAM, 0 );
  if(sock < 0)
  {
    fprintf(stderr,"创建通讯服务失败(socket()),errno=%d,%s\n",errno,strerror(errno));
    return -1;
  }

  memset(&addr,0,sizeof(struct sockaddr_in));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl ( INADDR_ANY );
  addr.sin_port = htons(portNo);

  opt = 1; 

#ifdef SO_REUSEADDR
  r = setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt));
  if (r < 0)
  {
    fprintf(stderr,"设置通讯参数失败(setsockopt()),errno=%d,%s",errno,strerror(errno));
    close(sock);
    return -1;
  }
#endif

#ifdef SO_REUSEPORT
  r = setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,(char *)&opt,sizeof(opt));
  if (r < 0)
  {
    fprintf(stderr,"设置通讯参数失败(setsockopt()),errno=%d,%s",errno,strerror(errno));
    close(sock);
    return -1;
  }
#endif

  if ( bind ( sock, ( struct sockaddr * ) &addr, sizeof addr ) < 0 )
  {
    fprintf(stderr,"绑定通讯服务失败(bind()),errno=%d,%s",errno,strerror(errno));
    close(sock);
    return -1;
  }

  if ( listen ( sock, 15 ) < 0 )
  {
    fprintf(stderr,"通讯服务监听失败(listen()),errno=%d,%s",errno,strerror(errno));
    close(sock);
    return -1;
  }

  return sock;
}

static void serverloop()
{
  struct sockaddr_in  localaddr,cliaddr;
  int                 cliaddrlen;
  int                 childpid;
  int                 newsock;
  int                 rv;

  for (; ; )
  {
    cliaddrlen = sizeof cliaddr;
    memset ( & cliaddr, 0, cliaddrlen );
    errno = 0;
    newsock = accept ( server_sock, ( struct sockaddr * )&cliaddr,(size_t *)&cliaddrlen );
    if ( newsock < 0 )
    {
      logger(__FILE__,__LINE__,"IBserver accept,errno %d %s", errno ,strerror(errno));
      continue;
    }
    errno = 0;
    if ( ( childpid = fork ( ) ) < 0 )
    {
      close ( newsock );
      logger(__FILE__,__LINE__,"IBserver can't fork,errno %d %s", errno ,strerror(errno));
      continue;
    }
    if ( childpid == 0 )  /* child process */
    {
      runserver ( newsock,&cliaddr );
      close ( newsock );
      exit ( 0 );
    }

    close ( newsock );
  }
}

static int checkconnection(Connection *conn,struct sockaddr_in *cliaddr)
{
  return 0;
}

static int runserver(int sock,struct sockaddr_in *cliaddr)
{
  Connection  connection;
  int         rv;
  char       *executeProgram;
  char        group[10],transMode[2];
  int         mynice;
  int         fc_controler;
  int         mac_error = 0;

  memset(&connection,0,sizeof(Connection));
  memset(group,0,sizeof(group));
  memset(transMode,0,sizeof(transMode));

  connection.sock = sock;

  rv = RecvHeader(&connection);
  if (rv != 0 && rv != 1)
  {
    close(sock);
    return -1;
  }

  if (rv == 1)
  {
    mac_error = 1;
  }

  rv = checkconnection(&connection,cliaddr);
  if (rv != 0)
  {
    SendHeaderAck(&connection,IB_CLIENT_IS_NOT_AUTH);
    close(sock);
    return -1;
  }

  if (mac_error == 1)
  {
    SendHeaderAck(&connection,IB_DATA_MAC_ERROR);
    close(sock);
    return -1;
  }

  executeProgram = getProgramName(connection.message_type,connection.trans_code,transMode,group);
  if( executeProgram == NULL )
  {
    SendHeaderAck(&connection,IB_SERVICE_NOTE_FOUND);
    close(sock);
    return -1;
  }

  rv = SendHeaderAck(&connection,0);
  if(rv)
  {
    close(sock);
    return -1;
  }

  IBsetenv(&connection);

  {
    int    status = 0;
    pid_t  pid;
    signal ( SIGCLD, SIG_DFL );

    if(semaphoreInit(0)|| semaphoreLock())
    {
      logger(__FILE__,__LINE__,"IBserver can't semaphore,errno %d,%s",errno,strerror(errno));
      close(sock);
      return(-1);
    }

    IBProcessAdd(group,&mynice);
    semaphoreUnlock();

    pid = fork();
    if(pid < 0)
    {
      logger(__FILE__,__LINE__,"IBserver can't fork,errno %d,%s",errno,strerror(errno));
      close(sock);
      exit(1);
    }
    if(pid == 0)
    {
      nice(mynice);
      executeThisProgram(executeProgram);
      close(sock);
      exit(1);
    }
    else
    {
      close(sock);
      status = 0;

      pid = waitpid(pid, &status, 0);
      if (WIFSIGNALED(status))
      {
        logger(__FILE__,__LINE__,"IBserver EXEC[%s] SIGNALED[%d]%s",executeProgram,
                    WTERMSIG(status),
#ifdef WCOREDUMP
                    WCOREDUMP(status)?"(core)":""
#else
                    ""
#endif
        );
      }
      if(semaphoreInit(0)|| semaphoreLock())
      {
        logger(__FILE__,__LINE__,"IBserver can't semaphore,errno %d,%s",errno,strerror(errno));
        close(sock);
        return(-1);
      }
  
      IBProcessMove(group);
      semaphoreUnlock();
      return 0;
    }
  }
  return -1;
}

void Term_signal(int sig_no)
{
  close(server_sock);
  semaphoreDel();
  exit(1);
}

int main()
{
  extern char  *CurrentProgram;

  CurrentProgram = "IBserver";

  if(semaphoreInit(1))
  {
    fprintf(stderr,"\n创建全局资源锁失败(semaphoreInit()),errno %d,%s\n",errno,strerror(errno));
    exit(0);
  }

  server_sock = IBserver();
  if (server_sock < 0)
  {
    semaphoreDel();
    fprintf(stderr,"\n IBserver 启动失败,请检查原因!\n");
    exit(0);
  }

  if(fork() != 0) 
    exit(0);

  setsid();
  signal(SIGTERM,Term_signal);
  signal ( SIGINT, SIG_IGN );
  signal ( SIGPIPE, SIG_IGN );
  signal ( SIGQUIT, SIG_IGN );
  signal ( SIGHUP, SIG_IGN );
  signal ( SIGCLD, SIG_IGN );

  serverloop();
  
  semaphoreDel();

  return 0;
}


