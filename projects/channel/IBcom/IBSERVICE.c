#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "IBcom.h"
#include "logger.h"
#include "ServiceBuf.h"
#include "IBerr.h"

static Connection connection;

static SERVICEFIELD  SBUF_SYS_MESSAGE_TYPE = {"MESSAGE_TYPE",FLDTYPE_STR};
static SERVICEFIELD  SBUF_SYS_TRANS_CODE = {"TRANS_CODE",FLDTYPE_STR};
static SERVICEFIELD  SBUF_SYS_REMOTE_NODE_ID = {"REMOTE_NODE_ID",FLDTYPE_STR};


static int packetFunction(void *data,char *buf,int buflen,int *use,int in_out)
{
  int  rv,len;

  switch (in_out)
  {
    case PACKET_IN :
      rv = ServiceBufToStream((SERVICEBUF *)data,buflen,use,buf);
      if (rv != 0)
      {
        logger(__FILE__,__LINE__,"服务转换报文失败(ServiceBufToStream) ,错误码{%d}",rv);
        return IB_ERR_SERVICEBUF_ERROR;
      }
      break;
    case PACKET_OUT :
      rv = StreamToServiceBuf((SERVICEBUF *)data,buflen,use,buf);
      if (rv != 0)
      {
        logger(__FILE__,__LINE__,"服务转换报文失败(StreamToServiceBuf) ,错误码{%d}",rv);
        return IB_ERR_SERVICEBUF_ERROR;
      }
      break;
    default:
      return -1;
  }

  return 0;
}

int IB_SERVICE_INIT(SERVICEBUF *sBuf,char *rcvFile)
{
  int   rv;

  rv = IBacceptConnect(&connection);
  if(rv != 0)
  {
    logger(__FILE__,__LINE__,"服务接受请求失败(IBacceptConnect) ,错误码{%d}",rv);
    return rv;
  }

  connection.pkg_type = 'S';

  rv = IBrcvData(&connection,rcvFile,
                 (void *)sBuf,
                 &packetFunction,
                 NULL);
  if (rv)
  {
    logger(__FILE__,__LINE__,"服务接受请求数据(IBrcvData) ,错误码{%d}",rv);
    IBdisconnect(&connection);
    return rv;
  }

  return rv;
}

int IB_SERVICE_INIT_EX(SERVICEBUF *sBuf,char *rcvFile)
{
  int   rv;

  rv = IBacceptConnectEx(&connection);
  if(rv != 0)
  {
    logger(__FILE__,__LINE__,"服务接受请求失败(IBacceptConnect) ,错误码{%d}",rv);
    return rv;
  }

  connection.pkg_type = 'S';

  rv = IBrcvDataEx(&connection,rcvFile,
                 (void *)sBuf,
                 &packetFunction,
                 NULL);
  if (rv)
  {
    logger(__FILE__,__LINE__,"服务接受请求数据(IBrcvData) ,错误码{%d}",rv);
    IBdisconnect(&connection);
    return rv;
  }

  return rv;
}

int IB_SERVICE_RETURN(SERVICEBUF *sBuf,char *sndFile)
{
  int  rv;  

  rv = IBsndData(&connection,sndFile,
                 (void *)sBuf,
                 &packetFunction,
                 NULL);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"服务发送应答数据(IBsndData) ,错误码{%d}",rv);
    IBdisconnect(&connection);
    return rv;
  }
  
  IBdisconnect(&connection);
  
  return 0;
}

int IB_SERVICE_RETURN_EX(SERVICEBUF *sBuf,char *sndFile)
{
  int  rv;  

  rv = IBsndDataEx(&connection,sndFile,
                 (void *)sBuf,
                 &packetFunction,
                 NULL);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"服务发送应答数据(IBsndData) ,错误码{%d}",rv);
    IBdisconnect(&connection);
    return rv;
  }
  
  IBdisconnect(&connection);
  
  return 0;
}

int IB_SERVICE_ABORT()
{
  IBdisconnect(&connection);

  return 0;
}


static int IB_GET_CLIENT_ENV(void *data,char *msgType,char *node,char *tranCode)

{
  SERVICEBUF *pubData = (SERVICEBUF *)data;
  int  rv,size;

  rv = GetServiceField(pubData,&SBUF_SYS_MESSAGE_TYPE,msgType,&size,MSG_TYPE_LEN);
  if (rv != 0)
  {
    return IB_ERR_SERVICEBUF_ERROR;
  }

  rv = GetServiceField(pubData,&SBUF_SYS_TRANS_CODE,tranCode,&size,TRANS_CODE_LEN);
  if (rv != 0)
  {
    return IB_ERR_SERVICEBUF_ERROR;
  }

  rv = GetServiceField(pubData,&SBUF_SYS_REMOTE_NODE_ID,node,&size,NODE_MAXLEN);
  if (rv != 0)
  {
    return IB_ERR_SERVICEBUF_ERROR;
  }

  return 0;
}

int IB_SERVICE_CALL(SERVICEBUF *sndBuf,SERVICEBUF *rcvBuf,char *sndFile,char *rcvFile)
{
  int        rv;
  Connection conn;

  rv = IBconnect(&conn,(void *)sndBuf,&IB_GET_CLIENT_ENV);
  if(rv != 0)
  {
    logger(__FILE__,__LINE__,"节点连接服务失败(IBconnect) ,错误码{%d}",rv);
    goto E;
  }

  conn.pkg_type = 'S';

  rv = IBsndData(&conn,sndFile,
                 (void *)sndBuf,
                 &packetFunction,
                 NULL);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"节点发送请求数据(IBsndData) ,错误码{%d}",rv);
    goto E;
  }

  rv = IBrcvData(&conn,rcvFile,
                (void *)rcvBuf,
                 &packetFunction,
                NULL);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"节点接受应答数据(IBrcvData) ,错误码{%d}",rv);
    goto E;
  }

E:
  IBdisconnect(&conn);
  return rv;
}

int IB_SERVICE_CALL_EX(SERVICEBUF *sndBuf,SERVICEBUF *rcvBuf,char *sndFile,char *rcvFile)
{
  int        rv;
  Connection conn;

  rv = IBconnectEx(&conn,(void *)sndBuf,&IB_GET_CLIENT_ENV);
  if(rv != 0)
  {
    logger(__FILE__,__LINE__,"节点连接服务失败(IBconnect) ,错误码{%d}",rv);
    goto E;
  }

  conn.pkg_type = 'S';

  rv = IBsndDataEx(&conn,sndFile,
                 (void *)sndBuf,
                 &packetFunction,
                 NULL);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"节点发送请求数据(IBsndData) ,错误码{%d}",rv);
    goto E;
  }

  rv = IBrcvDataEx(&conn,rcvFile,
                (void *)rcvBuf,
                 &packetFunction,
                NULL);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"节点接受应答数据(IBrcvData) ,错误码{%d}",rv);
    goto E;
  }

E:
  IBdisconnect(&conn);
  return rv;
}




