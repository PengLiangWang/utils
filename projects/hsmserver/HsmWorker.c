#include "ttsys.h"
#include "HsmCommand.h"
#include "HsmRes.h"

static TMutex    GMutex;

static TSocket  *GWorkerSocket = NULL;

static void HsmWorkerStop(int sig)
{
  ELOG(ERROR, "recv sig SIGUSER2");
  sigset(SIGUSR2, SIG_IGN);
  SocketClose(&GWorkerSocket);
  exit(sig);
}

int HsmWorker(int limit, TSocket *SereserSocket)
{
  int       count = 0;
  TSocket  *socket;
  int       res;
  
  GWorkerSocket = SereserSocket;

  HsmResGetMutex(&GMutex);

  signal( SIGINT,  SIG_IGN );
  signal( SIGPIPE, SIG_IGN );
  signal( SIGQUIT, SIG_IGN );
  signal( SIGHUP,  SIG_IGN );
  signal( SIGTERM, SIG_IGN );
  signal( SIGCLD,  SIG_IGN );
  sigset( SIGUSR2, HsmWorkerStop);

  for (; ; )
  {
    if ( ( res = MutexAcquire(&GMutex,1) ))
    {
      SocketClose(&GWorkerSocket);
      return res;
    }
    
    res = SocketAccept(&socket,GWorkerSocket);
    if ( res != TTS_SUCCESS )
    {
      MutexRelease(&GMutex,1);
      return res;
    }

    MutexRelease(&GMutex,1);

    //sighold(SIGUSR2);
    
    SecurityWorkerMain(socket);
    
    count++;
    
    //sigrelse(SIGUSR2);
    
    if ( count > limit )
    {
      SocketClose(&GWorkerSocket);
      break;
    }
  }

  return TTS_SUCCESS;
}


static char  GRecvBuffer[2048];
static char  GSendBuffer[2048];

int SecurityWorkerMain( TSocket *newsock)
{
  int           res;
  char          cmd[3];
  int           len;
  char         *inData;
  char         *outData;
  char          lenBuf[3];

  for ( ; ; )
  {
    memset(GRecvBuffer,0,sizeof(GRecvBuffer));
    memset(GSendBuffer,0,sizeof(GSendBuffer));
    memset(cmd,0,sizeof(cmd));

    len = 2;
    memset(lenBuf,0,sizeof(lenBuf));

    res = SocketRecv(newsock,lenBuf,len);
    if ( res != TTS_SUCCESS )
    {
      SocketClose(&newsock);
      return res;
    }

    len = (int)(lenBuf[0] & 0xFF) * 0xFF + (int)(lenBuf[1] & 0xFF) ;
    
    if ( len == 0 )
    {
      SocketClose(&newsock);
      return res;
    }

    res = SocketRead(newsock,GRecvBuffer,len,5);
    if ( res != TTS_SUCCESS )
    {
      ELOG(ERROR, "接受请求客户端请求失败(%s:%d)", GetStrError(res), res);
      SocketClose(&newsock);
      return res;
    }

    if ( elogisdebug() )
    {
      ELOG(DEBUG, "RECV: %s",GRecvBuffer);
    }

    strncpy(cmd,GRecvBuffer,2);
    inData = GRecvBuffer + 2;
    len -= 2;
    outData = GSendBuffer;

    HsmExecCommand(cmd,inData,len,outData);

    if ( elogisdebug() )
    {
      ELOG(DEBUG, "SEND: %s",GSendBuffer);
    }

    len = strlen(GSendBuffer);
    
    res = SocketWriteEx(newsock,GSendBuffer,len,10,SOCKET_HEX_LENGTH);
    if (res)
    {
      ELOG(ERROR, "返回客户端应答失败(%s:%d)", GetStrError(res), res);
      SocketClose(&newsock);
      return res;
    }
  }
}

