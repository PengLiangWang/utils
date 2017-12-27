#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "IBcom.h"
#include "logger.h"
#include "RCI.h"

static Connection connection;

static int IB_RCI_RECV_(RCI_ENV *rcienv,void *privateSData,
                   int (* packetFunction)(void *data,char *buf,int buflen,int *use,int in_out),
                   char *rcvFile)
{
  int   rv;

  connection.pkg_type = 'R';
  rv = IBrcvData(&connection,rcvFile,
                 (void *)rcienv,
                 &IB_RCI_PKG_ENV,
                 privateSData,
                 packetFunction,
                 NULL);
  if (rv)
  {
    goto E;
  }
E:

  return rv;
}

int IB_RCI_INIT(RCI_ENV *rcienv,void *rcvData,
              int (* packetFunction)
              (void *data,char *buf,int buflen,int *use,int in_out),
              char *rcvFile)
{
  int   rv;

  rv = IBacceptConnect(&connection);
  if(rv != 0)
  {
    return rv;
  }

  return IB_RCI_RECV_(rcienv,rcvData,packetFunction,rcvFile);
}

static int IB_RCI_SEND_(RCI_ENV *rcienv,void *privateSData,
                       int (* packetFunction)
                       (void *data,char *buf,int buflen,int *use,int in_out),
                       char *sndFile)
{
  int rv;

  rv = IBsndData(&connection,sndFile,
                 (void *)rcienv,
                 &IB_RCI_PKG_ENV,
                 privateSData, 
                 packetFunction,
                 NULL);
  return rv;
}

int IB_RCI_RETURN(RCI_ENV *rcienv,void *sndData,
                  int (* packetFunction)
                  (void *data,char *buf,int buflen,int *use,int in_out),
                  char *sndFile)
{
  int rv;  

  rv = IB_RCI_SEND_(rcienv, sndData,packetFunction, sndFile);
  if ( rv != 0 )
  {
    logger(__FILE__,__LINE__,"IB IB_RCI_RETURN %d",rv);
  }
  
  rv = IBdisconnect(&connection);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IB IBdisconnect[%d]",rv);
  }

  return rv;
}

static int IB_GET_CLIENT_ENV(void *data,char *msgType,char *node,char *tranCode)

{
  RCI_ENV *pubData = (RCI_ENV *)data;
  strncpy(node,pubData->remote_node_id,NODE_MAXLEN);
  strncpy(msgType,pubData->message_type,MSG_TYPE_LEN);
  strncpy(tranCode,pubData->trans_code,TRANS_CODE_LEN); 
  return 0;
}

static int RCI_CALL_(RCI_ENV *rcienv,
    void *privateSndData,
    void *privateRcvData,
    int (* packetInFunction)
      (void *data,char *buf,int buflen,int *use,int in_out),
    int (* packetOutFunction)
      (void *data,char *buf,int buflen,int *use,int in_out),
    char *sndFile,
    char *rcvFile)
{
  int        rv;
  Connection conn;

  rv = IBconnect(&conn,(void *)rcienv,&IB_GET_CLIENT_ENV);
  if(rv)
    goto E;

  conn.pkg_type = 'R';

  rv = IBsndData(&conn,sndFile,
                 (void *)rcienv,
                 &IB_RCI_PKG_ENV,
                 privateSndData,
                 packetInFunction,
                 NULL);

  if(rv)
    goto E;

  rv = IBrcvData(&conn,rcvFile,
                (void *)rcienv,
                &IB_RCI_PKG_ENV,
                privateRcvData,  
                packetOutFunction,
                NULL);
E:
  IBdisconnect(&conn);
  return rv;
}

int IB_RCI_CALL(RCI_ENV *rci,void *sndData, void *rcvData,
                int (* packetInFunction)
                  (void *data,char *buf,int buflen,int *use,int in_out),
                int (* packetOutFunction)
                  (void *data,char *buf,int buflen,int *use,int in_out),
                char *sndFile, char *rcvFile)
{
  int  rv;  

  rv = RCI_CALL_( rci, sndData, rcvData, packetInFunction, packetOutFunction, 
                     sndFile, rcvFile);
  if (rv != 0)
    logger(__FILE__,__LINE__,"IB CALL err[%d]",rv);

  return rv;
}
