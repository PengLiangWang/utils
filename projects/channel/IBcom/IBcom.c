#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <signal.h>
#include "IBcom.h"
#include "DesUtil.h"
#include "logger.h"
#include "filename.h"
#include "IBerr.h"
#include "IBnote.h"
extern int errno;

#define ENV_SOCKN    "ENV_SOCKN"
#define ENV_NODE     "ENV_NODE"

int writeN(int fd,char *pcBuffer,int len,int timeout)
{
  int             n = 0;
  int             err = 0;
  fd_set          writefds;
  int             found ;
  long            startTime,currentTime;
  struct timeval  tt;


  FD_ZERO(&writefds);
  time(&startTime);

  while((len > 0) && (err == 0))
  {
    time(&currentTime);
    if((currentTime - startTime) > timeout)
    {
      logger(__FILE__,__LINE__,"writeN()=>currentTime - startTime > timeout:%d",timeout);
      err = IB_ERR_SEND_TIMEOUT;
    }
    else
    {
      FD_SET(fd,&writefds);
      tt.tv_sec = timeout - (currentTime - startTime);
      tt.tv_usec = 0;
      found = select(fd + 1,NULL,&writefds,NULL,&tt);
      if(found == 0)
      {
        logger(__FILE__,__LINE__,"writeN() => found=0,errno=%d,%s",errno,strerror(errno));
        err = IB_ERR_SEND_TIMEOUT;
      }
      else if(found < 0)
      {
        logger(__FILE__,__LINE__,"writeN() => select(),errno=%d,%s",errno,strerror(errno));
        err = IB_ERR_SEND_ERROR;
      }
      else
      {
        n = write(fd,pcBuffer,len);
        if(n <= 0)
        {
          logger(__FILE__,__LINE__,"writeN()=>write(),errno=%d,%s",errno,strerror(errno));
          err = IB_ERR_SEND_ERROR;
        }
        else
        {
          len -= n;
          pcBuffer += n;
        }
      }
    }
  }

  return err;
}

int readN(int fd,char *pcBuffer,int len,int timeout)
{
  int             n = 0;
  int             err = 0;
  fd_set          readfds;
  int             found  = 0;
  long            startTime = 0,currentTime = 0;
  struct timeval  tt;

  FD_ZERO(&readfds);
  time(&startTime);
  errno = 0;
  while((len > 0) && (err == 0))
  {
    time(&currentTime);
    if((currentTime - startTime) > timeout)
    {
      logger(__FILE__,__LINE__,"readN()=> currentTime - startTime) > timeout:%d",timeout);
      err = IB_ERR_RECV_TIMEOUT;
    }
    else
    {
      FD_SET(fd,&readfds);
      tt.tv_sec = timeout - (currentTime - startTime);
      tt.tv_usec=0;
      found = select(fd+1,&readfds,NULL,NULL,&tt);
      if(found == 0)
      {
        logger(__FILE__,__LINE__,"readN()=>found = 0 ,errno=%d,%s",errno,strerror(errno));
        err = IB_ERR_RECV_TIMEOUT;
      }
      else if(found < 0)
      {
        logger(__FILE__,__LINE__,"readN()=>select(),errno=%d,%s",errno,strerror(errno));
        err = IB_ERR_RECV_ERROR;
      }
      else
      {
        n = read(fd,pcBuffer,len);
        if(n <= 0)
        {
          logger(__FILE__,__LINE__,"readN()=>read(),errno=%d,%s",errno,strerror(errno));
          err = IB_ERR_RECV_ERROR;
        }
        else
        {
          len -= n;
          pcBuffer += n;
        }
      }
    }
  }

  return err;
}

int SetBlockHeader(char *dataHead,int dataSize,char blockType,char hasData)
{
  sprintf(dataHead,"%04d",dataSize);
  dataHead += 4;
  *dataHead++ = 'N';
  *dataHead++ = 'F';
  *dataHead++ = blockType;
  *dataHead++ = hasData;
  *dataHead   = 'E';

  return 0;
}

int GetBlockHeader(char *dataHead,int *dataSize,char *blockType,char *hasData)
{
  char length[5];

  if(dataHead[4] != 'N' && dataHead[5] != 'F' && dataHead[8] != 'E')
  {
    logger(__FILE__,__LINE__,"GetBlockHeader() error,%c,%c,%c",
                                dataHead[4],dataHead[5],dataHead[8]);
    return IB_ERR_BAD_DATA_HEAD;
  }

  *blockType = dataHead[6];
  *hasData   = dataHead[7];
  
  memset(length,0,sizeof(length));
  memcpy(length,dataHead,4);
  *dataSize = atoi(length);
  
  return 0;
}

static char dataBuf[IB_MAXDATABLOCK];

int SendHeader(Connection *conn)
{
  int   rv;
  char  head[ NODE_MAXLEN * 2+ MSG_TYPE_LEN + TRANS_CODE_LEN + 1];

  memset(head,0,sizeof(head));

  strncpy(head,conn->message_type,MSG_TYPE_LEN);
  strncpy(head+MSG_TYPE_LEN,conn->trans_code,TRANS_CODE_LEN);
  strncpy(head+MSG_TYPE_LEN+TRANS_CODE_LEN,conn->local_node_id,NODE_MAXLEN);
  strncpy(head+MSG_TYPE_LEN+TRANS_CODE_LEN+NODE_MAXLEN,conn->remote_node_id,NODE_MAXLEN);
  

  rv = writeN(conn->sock,head,NODE_MAXLEN*2+MSG_TYPE_LEN+TRANS_CODE_LEN,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"SendHeader()=>writeN() error:%d",rv);
    return rv;
  }

  return 0;
}

int RecvHeaderAck(Connection *conn,int *response)
{
  char  ack[5];
  int   rv;

  memset(ack,0,sizeof(ack));
  
  rv = readN(conn->sock,ack,4,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvHeaderAck()=>readN(),error %d",rv);
    return rv;
  }

  *response = atoi(ack);
  
  return *response;

}

int RecvHeader(Connection *conn)
{
  int   rv;
  char  head[ NODE_MAXLEN * 2+ MSG_TYPE_LEN + TRANS_CODE_LEN + 1];
  int   len;

  memset(head,0,sizeof(head));

  rv = readN(conn->sock,head,NODE_MAXLEN*2+MSG_TYPE_LEN+TRANS_CODE_LEN,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvHeader()=>readN(),error %d",rv);
    return rv;
  }

  strncpy(conn->message_type,head,MSG_TYPE_LEN);
  strncpy(conn->trans_code,head+MSG_TYPE_LEN,TRANS_CODE_LEN);
  strncpy(conn->remote_node_id,head+MSG_TYPE_LEN+TRANS_CODE_LEN,NODE_MAXLEN);
  strncpy(conn->local_node_id,head+MSG_TYPE_LEN+TRANS_CODE_LEN+NODE_MAXLEN,NODE_MAXLEN);

  return 0;
}

int SendHeaderAck(Connection *conn,int response)
{
  int   rv;
  char  ack[5];

  memset(ack,0,sizeof(ack));
  
  sprintf(ack,"%04d",response);

  rv = writeN(conn->sock,ack,4,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"SendHeaderAck()=>writeN() error %d",rv);
    return rv;
  }

  return 0;
}

int IB_TRANS_TIMEOUT = IB_DEFAULT_TIMEOUT;

int RecvDataBlock(Connection *conn,char *Buf,int maxlen,int *len)
{
  int      dataSize;
  char     blockType;
  char     hasData;
  int      rv;
  char     MAC[9];

  
  rv = readN(conn->sock,Buf,9,IB_TRANS_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvDataBlock()=>readN(data block head) %d",rv);
    return rv;
  }
  
  rv = GetBlockHeader(Buf,&dataSize,&blockType,&hasData);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvDataBlock()=>GetBlockHeader() %d",rv);
    logmsg_ex(__FILE__,__LINE__,"数据头信息非法:",Buf,9);
    return IB_ERR_RECV_ERROR;
  }
 
  if(dataSize > maxlen)
  {
    logger(__FILE__,__LINE__,"RecvDataBlock()=> dataSize(%d) > %d",dataSize,maxlen);
    return IB_ERR_DATA_LEN;
  }

  conn->has_rcv_file = (hasData == 'F') ? 1 : 0;

  *len = dataSize;

  if (dataSize > 0)
  {
    memset(MAC,0,sizeof(MAC));

    rv = readN(conn->sock,MAC,8,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvDataBlock()=> readN(data block) MAC %d",rv);
      return rv;
    }

    rv = readN(conn->sock,Buf + 9,dataSize,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvDataBlock()=> readN(data block) %d",rv);
      return rv;
    }

    rv = IBsecCheckMac(conn->remote_node_id,Buf + 9,dataSize,MAC);
    if (rv == 1)
    {
      logger(__FILE__,__LINE__,"节点%s非法数据入侵分析MAC=%s",conn->remote_node_id,MAC);
      logmsg_ex(__FILE__,__LINE__,"IMG数据:",Buf,dataSize + 9);
      return IB_ERR_MAC_ERROR;
    }
    else if (rv)
    {
      logger(__FILE__,__LINE__,"RecvDataBlock()=> IBsecCheckMac(data block) %d",rv);
      return IB_ERR_ENCRYPT_ERROR;
    }
    
    rv = IBsecDedesBlock(conn->remote_node_id,Buf + 9,dataSize);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvDataBlock()=> IBsecDedesBlock() %d",rv);
      return IB_ERR_ENCRYPT_ERROR;
    }
  }

#ifdef _IB_DEBUG_
  logmsg_ex(__FILE__,__LINE__,"RECV BLOCK DATA",Buf,*len);
#endif  

  return 0;
}

int RecvDataBlockEx(Connection *conn,char *Buf,int maxlen,int *len)
{
  int      dataSize;
  char     blockType;
  char     hasData;
  int      rv;
  char     MAC[9];

  
  rv = readN(conn->sock,Buf,9,IB_TRANS_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvDataBlock()=>readN(data block head) %d",rv);
    return rv;
  }
  
  rv = GetBlockHeader(Buf,&dataSize,&blockType,&hasData);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvDataBlock()=>GetBlockHeader() %d",rv);
    logmsg_ex(__FILE__,__LINE__,"数据头信息非法:",Buf,9);
    return IB_ERR_RECV_ERROR;
  }
 
  if(dataSize > maxlen)
  {
    logger(__FILE__,__LINE__,"RecvDataBlock()=> dataSize(%d) > %d",dataSize,maxlen);
    return IB_ERR_DATA_LEN;
  }

  conn->has_rcv_file = (hasData == 'F') ? 1 : 0;

  *len = dataSize;

  if (dataSize > 0)
  {
    
    rv = readN(conn->sock,Buf + 9,dataSize,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvDataBlock()=> readN(data block) %d",rv);
      return rv;
    }
  }

  return 0;
}

int RecvBlockData(Connection *conn,char *buffer,int *len)
{
  int      dataSize;
  char     blockType;
  char     hasData;
  int      rv;
  char     Buf[IB_MAXDATABLOCK + 1];
  char     MAC[9];

  memset(Buf,0,sizeof(Buf));

  rv = readN(conn->sock,Buf,9,IB_TRANS_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=>readN(data block head) %d",rv);
    return rv;
  }
  
  rv = GetBlockHeader(Buf,&dataSize,&blockType,&hasData);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=>GetBlockHeader() %d",rv);
    return rv;
  }
 
  if(dataSize > IB_MAXDATABLOCK)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=> dataSize(%d) > 8192",dataSize);
    return IB_ERR_DATA_LEN;
  }
  conn->has_rcv_file = (hasData == 'F') ? 1 : 0;

  *len = dataSize;

  if (dataSize > 0)
  {
    memset(MAC,0,sizeof(MAC));

    rv = readN(conn->sock,MAC,8,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvDataBlock()=> readN(data block) MAC %d",rv);
      return rv;
    }

//#ifdef _IB_DEBUG_
  logmsg_ex(__FILE__,__LINE__,"数据MAC",MAC,8);
//#endif

    rv = readN(conn->sock,Buf + 9,dataSize,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvBlockData()=> readN(data block) %d",rv);
      return rv;
    }

    rv = IBsecCheckMac(conn->remote_node_id,Buf + 9,dataSize,MAC);
    if (rv == 1)
    {
      logger(__FILE__,__LINE__,"节点%s非法数据入侵分析",conn->remote_node_id);
      logmsg_ex(__FILE__,__LINE__,"IMG数据:",Buf,dataSize + 9);
      return IB_ERR_MAC_ERROR;
    }
    else if (rv)
    {
      logger(__FILE__,__LINE__,"RecvBlockData()=> IBsecCheckMac(data block) %d",rv);
      return IB_ERR_ENCRYPT_ERROR;
    }

    rv = IBsecDedesBlock(conn->remote_node_id,Buf + 9,dataSize);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvBlockData()=> IBsecDedesBlock() %d",rv);
      return IB_ERR_ENCRYPT_ERROR;
    }    
  }
  
  memmove(buffer,&Buf[9],dataSize);

#ifdef _IB_DEBUG_
  logmsg_ex(__FILE__,__LINE__,"RCV DATA BLOCK",Buf,*len);
#endif  

  if (blockType != conn->pkg_type)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=> BlockType[%c] != CONN->PKG_TYPE[%c]",blockType,conn->pkg_type);
    return IB_ERR_PKG_TYPE_ERROR;
  }

  return 0;
}

int RecvBlockDataEx(Connection *conn,char *buffer,int *len)
{
  int      dataSize;
  char     blockType;
  char     hasData;
  int      rv;
  char     Buf[IB_MAXDATABLOCK + 1];

  memset(Buf,0,sizeof(Buf));

  rv = readN(conn->sock,Buf,9,IB_TRANS_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=>readN(data block head) %d",rv);
    return rv;
  }
  
  rv = GetBlockHeader(Buf,&dataSize,&blockType,&hasData);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=>GetBlockHeader() %d",rv);
    return rv;
  }
 
  if(dataSize > IB_MAXDATABLOCK)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=> dataSize(%d) > 8192",dataSize);
    return IB_ERR_DATA_LEN;
  }
  conn->has_rcv_file = (hasData == 'F') ? 1 : 0;

  *len = dataSize;

  if (dataSize > 0)
  {
    
    rv = readN(conn->sock,Buf + 9,dataSize,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"RecvBlockData()=> readN(data block) %d",rv);
      return rv;
    }

  }
  
  memmove(buffer,&Buf[9],dataSize);


  if (blockType != conn->pkg_type)
  {
    logger(__FILE__,__LINE__,"RecvBlockData()=> BlockType[%c] != CONN->PKG_TYPE[%c]",blockType,conn->pkg_type);
    return IB_ERR_PKG_TYPE_ERROR;
  }

  return 0;
}

int SendBlockData(Connection *conn,char *buffer,int len)
{
  int   rv;
  char  MAC[9];

  memmove(&dataBuf[17],buffer,len);
  
  SetBlockHeader(dataBuf,len,(char)conn->pkg_type,(char)(conn->has_snd_file ? 'F' : '0'));

  rv = IBsecEndesBlock(conn->remote_node_id,dataBuf + 17,len);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"SendBlockData()=> IBsecEndesBlock() %d",rv);
    return IB_ERR_ENCRYPT_ERROR;
  }

  memset(MAC,0,sizeof(MAC));

  rv = IBsecCreateMac(conn->remote_node_id,dataBuf + 17,len,MAC);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"SendBlockData()=> IBsecCreateMac() %d",rv);
    return IB_ERR_ENCRYPT_ERROR;
  }
  
  memmove(&dataBuf[9],MAC,8);

  rv = writeN(conn->sock,dataBuf,len  + 17,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"SendBlockData()=> writeN() error %d",rv);
    return rv;
  }

#ifdef _IB_DEBUG_  
  logmsg_ex(__FILE__,__LINE__,"SND DATA BLOCK",dataBuf,len  + 9);
#endif
  
  return 0;
}

int SendBlockDataEx(Connection *conn,char *buffer,int len)
{
  int   rv;

  memmove(&dataBuf[9],buffer,len);
  
  SetBlockHeader(dataBuf,len,(char)conn->pkg_type,(char)(conn->has_snd_file ? 'F' : '0'));


  rv = writeN(conn->sock,dataBuf,len  + 9,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"SendBlockData()=> writeN() error %d",rv);
    return rv;
  }
  
  return 0;
}

int SendBlockAck(Connection *conn)
{
  char dataHead[10];
  int  rv ;

  memset(dataHead,0,sizeof(dataHead));

  SetBlockHeader(dataHead,0,'A','0');
  
  rv = writeN(conn->sock,dataHead,9,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"SendBlockAck() => writeN() %d",rv);
    return rv;
  }

  return 0;
}

int RecvBlockAck(Connection *conn)
{
  int  rv = 0;
  int  dataSize = 0;
  char blockType = 0;
  char hasData = 0;
  char dataHead[10];

  memset(dataHead,0,sizeof(dataHead));

  rv = readN(conn->sock,dataHead,9,IB_DEFAULT_TIMEOUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvBlockAck() => readN() %d",rv);
    return rv;
  }

  rv = GetBlockHeader(dataHead,&dataSize,&blockType,&hasData);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"RecvBlockAck() => GetBlockHeader() %d",rv);
    return rv;
  }

  if (blockType != 'A')
  {
    logger(__FILE__,__LINE__,"RecvBlockAck() => blockType!='A',blockType %c",blockType);
    return IB_ERR_RECV_ACK;
  }

  return 0;
}

int IBsndFile(Connection *conn,char *sndFile)
{
  struct stat  sfbuf;
  int          dataSize;
  int          fh;
  int          rv = 0,g,file_len = 0,size;
  char         hasData;
  char         res[3];
  char         MAC[9];

  if( sndFile[0] == '/' )
  {
    if(stat(sndFile,&sfbuf))
    {
      logger(__FILE__,__LINE__,"IBsndFile() => stat(%s) errno %d,%s",sndFile,errno,strerror(errno));
      return IB_ERR_OPEN_FILE_ERROR;
    }
    fh = open(sndFile,O_RDONLY);
  }
  else
  {
    if(stat(GetTransFileName(sndFile),&sfbuf))
    {
      logger(__FILE__,__LINE__,"IBsndFile() => stat(%s) errno %d,%s",sndFile,errno,strerror(errno));
      return IB_ERR_OPEN_FILE_ERROR;
    }
    fh = open(GetTransFileName(sndFile),O_RDONLY);
  }
  if(fh < 0)
  {
    logger(__FILE__,__LINE__,"IBsndFile() => open(%s) errno %d %s",sndFile,errno,strerror(errno));
    return IB_ERR_OPEN_FILE_ERROR;
  }

  g = 1;
  rv = 0;
  
  while(g)
  {
    memset(dataBuf,0,sizeof(dataBuf));
    memset(res,0,sizeof(res));

    size = read(fh,&dataBuf[17],IB_FILE_DATA_UINT);
    if(size < 0)
    {
      rv = IB_ERR_SND_FILE_ERROR;
      logger(__FILE__,__LINE__,"IBsndFile() => read() errno %d %s",errno,strerror(errno));
      g = 0;
    }
    else
    {
      file_len += size;
      dataSize  = size;
      
      hasData = (char)(sfbuf.st_size <= file_len ? '0' : 'F');
      SetBlockHeader(dataBuf,dataSize,'F',hasData);

      rv = IBsecEndesBlock(conn->remote_node_id,dataBuf + 17,dataSize);
      if (rv)
      {
        logger(__FILE__,__LINE__,"IBsndFile() => IBsecEndesBlock() error %d",rv);
        rv = IB_ERR_ENCRYPT_ERROR;
        g = 0;
      }

      memset(MAC,0,sizeof(MAC));

      rv = IBsecCreateMac(conn->remote_node_id,dataBuf + 17,dataSize,MAC);
      if (rv != 0)
      {
        logger(__FILE__,__LINE__,"SendBlockData()=> IBsecCreateMac() %d",rv);
        return IB_ERR_ENCRYPT_ERROR;
      }

      memmove(&dataBuf[9],MAC,8);

      rv = writeN(conn->sock,dataBuf,dataSize + 17,IB_DEFAULT_TIMEOUT);
      if (rv != 0)
      {
        logger(__FILE__,__LINE__,"IBsndFile() => wrtieN() error %d",rv);
        rv = IB_ERR_SND_FILE_ERROR;
        g = 0;
      }
      else
      {
        memset(res,0,sizeof(res));
        
        rv = readN(conn->sock,res,2,IB_DEFAULT_TIMEOUT);
        if (rv != 0)
        {
          logger(__FILE__,__LINE__,"IBsndFile() => readN(res) error %d",rv);
          rv = IB_ERR_SND_FILE_ERROR;
          g = 0;
        }
        else
        {
          if (strncmp(res,"00",2))
          {
            logger(__FILE__,__LINE__,"IBsndFile() => res[%s] != '00'",res);
            rv = IB_ERR_SND_FILE_ERROR;
            g = 0;
          }
        }
      }
      if (g != 0  && sfbuf.st_size == file_len)
      {
        g = 0;
      }
    }
  }

  close(fh);

  return rv;
  
}

int IBsndFileEx(Connection *conn,char *sndFile)
{
  struct stat  sfbuf;
  int          dataSize;
  int          fh;
  int          rv = 0,g,file_len = 0,size;
  char         hasData;
  char         res[3];

  if( sndFile[0] == '/' )
  {
    if(stat(sndFile,&sfbuf))
    {
      logger(__FILE__,__LINE__,"IBsndFile() => stat(%s) errno %d,%s",sndFile,errno,strerror(errno));
      return IB_ERR_OPEN_FILE_ERROR;
    }
    fh = open(sndFile,O_RDONLY);
  }
  else
  {
    if(stat(GetTransFileName(sndFile),&sfbuf))
    {
      logger(__FILE__,__LINE__,"IBsndFile() => stat(%s) errno %d,%s",sndFile,errno,strerror(errno));
      return IB_ERR_OPEN_FILE_ERROR;
    }
    fh = open(GetTransFileName(sndFile),O_RDONLY);
  }
  if(fh < 0)
  {
    logger(__FILE__,__LINE__,"IBsndFile() => open(%s) errno %d %s",sndFile,errno,strerror(errno));
    return IB_ERR_OPEN_FILE_ERROR;
  }

  g = 1;
  rv = 0;
  
  while(g)
  {
    memset(dataBuf,0,sizeof(dataBuf));
    memset(res,0,sizeof(res));

    size = read(fh,&dataBuf[9],IB_FILE_DATA_UINT);
    if(size < 0)
    {
      rv = IB_ERR_SND_FILE_ERROR;
      logger(__FILE__,__LINE__,"IBsndFile() => read() errno %d %s",errno,strerror(errno));
      g = 0;
    }
    else
    {
      file_len += size;
      dataSize  = size;
      
      hasData = (char)(sfbuf.st_size <= file_len ? '0' : 'F');
      SetBlockHeader(dataBuf,dataSize,'F',hasData);

      rv = writeN(conn->sock,dataBuf,dataSize + 9,IB_DEFAULT_TIMEOUT);
      if (rv != 0)
      {
        logger(__FILE__,__LINE__,"IBsndFile() => wrtieN() error %d",rv);
        rv = IB_ERR_SND_FILE_ERROR;
        g = 0;
      }
      else
      {
        memset(res,0,sizeof(res));
        
        rv = readN(conn->sock,res,2,IB_DEFAULT_TIMEOUT);
        if (rv != 0)
        {
          logger(__FILE__,__LINE__,"IBsndFile() => readN(res) error %d",rv);
          rv = IB_ERR_SND_FILE_ERROR;
          g = 0;
        }
        else
        {
          if (strncmp(res,"00",2))
          {
            logger(__FILE__,__LINE__,"IBsndFile() => res[%s] != '00'",res);
            rv = IB_ERR_SND_FILE_ERROR;
            g = 0;
          }
        }
      }
      if (g != 0  && sfbuf.st_size == file_len)
      {
        g = 0;
      }
    }
  }

  close(fh);

  return rv;
  
}

int IBrcvFile(Connection *conn,char *rcvFile)
{
  int    fh;
  int    size,dataSize,file_len,g,rv,len = 0;
  char   hasData,blockType;
  char   MAC[9];

  fh = open(GetTransFileName(rcvFile),O_WRONLY|O_CREAT|O_TRUNC,0600);
  if (fh < 0)
  {
    logger(__FILE__,__LINE__,"IBrcvFile()=>open(%s) errno %d %s",rcvFile,errno,strerror(errno));
    return IB_ERR_RCV_FILE_ERROR;
  }

  g = 1;
  rv = 0;

  while (g)
  {
    memset(dataBuf,0,sizeof(dataBuf));
    rv = RecvDataBlock(conn,dataBuf,IB_MAXDATABLOCK,&len);
    if (rv != 0 && rv != IB_ERR_MAC_ERROR)
    {
      logger(__FILE__,__LINE__,"IBrcvFile()=>RecvDataBlock() error %d",rv);
      break;
    }

    if (rv == IB_ERR_MAC_ERROR)
    {
      logger(__FILE__,__LINE__,"IBrcvFile() => 文件数据 MAC 检验失败.");

      rv = writeN(conn->sock,"96",2,IB_DEFAULT_TIMEOUT);
      if (rv != 0)
      {
        logger(__FILE__,__LINE__,"IBrcvFile() => writeN() error %d",rv);
        rv = IB_ERR_RCV_FILE_ERROR;
        break;
      }

      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }

    rv = GetBlockHeader(dataBuf,&dataSize,&blockType,&hasData);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"IBrcvFile()=>GetBlockHeader() error %d",rv);
      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }

    if (blockType != 'F')
    {
      logger(__FILE__,__LINE__,"IBrcvFile()=> blockType[%c] != 'F'() ",blockType);
      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }

    if (dataSize > 0)
    {
      size = write(fh,&dataBuf[9],dataSize);
      if (size != dataSize)
      {
        logger(__FILE__,__LINE__,"IBrcvFile() => write() errno %d %s",errno,strerror(errno));

        rv = writeN(conn->sock,"96",2,IB_DEFAULT_TIMEOUT);
        if (rv != 0)
        {
          logger(__FILE__,__LINE__,"IBrcvFile() => writeN() error %d",rv);
          rv = IB_ERR_RCV_FILE_ERROR;
          break;
        }

        rv = IB_ERR_RCV_FILE_ERROR;
        break;
      }
    }
    rv = writeN(conn->sock,"00",2,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"IBrcvFile() => writeN() error %d",rv);
      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }
    if (dataSize == 0 || hasData != 'F')
    {
      g = 0;
    }
  }
  close(fh);
  return 0;
}

int IBrcvFileEx(Connection *conn,char *rcvFile)
{
  int    fh;
  int    size,dataSize,file_len,g,rv,len = 0;
  char   hasData,blockType;

  fh = open(GetTransFileName(rcvFile),O_WRONLY|O_CREAT|O_TRUNC,0600);
  if (fh < 0)
  {
    logger(__FILE__,__LINE__,"IBrcvFile()=>open(%s) errno %d %s",rcvFile,errno,strerror(errno));
    return IB_ERR_RCV_FILE_ERROR;
  }

  g = 1;
  rv = 0;

  while (g)
  {
    memset(dataBuf,0,sizeof(dataBuf));
    rv = RecvDataBlockEx(conn,dataBuf,IB_MAXDATABLOCK,&len);
    if (rv != 0 && rv != IB_ERR_MAC_ERROR)
    {
      logger(__FILE__,__LINE__,"IBrcvFile()=>RecvDataBlock() error %d",rv);
      break;
    }

    if (rv == IB_ERR_MAC_ERROR)
    {
      logger(__FILE__,__LINE__,"IBrcvFile() => 文件数据 MAC 检验失败.");

      rv = writeN(conn->sock,"96",2,IB_DEFAULT_TIMEOUT);
      if (rv != 0)
      {
        logger(__FILE__,__LINE__,"IBrcvFile() => writeN() error %d",rv);
        rv = IB_ERR_RCV_FILE_ERROR;
        break;
      }

      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }

    rv = GetBlockHeader(dataBuf,&dataSize,&blockType,&hasData);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"IBrcvFile()=>GetBlockHeader() error %d",rv);
      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }

    if (blockType != 'F')
    {
      logger(__FILE__,__LINE__,"IBrcvFile()=> blockType[%c] != 'F'() ",blockType);
      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }

    if (dataSize > 0)
    {
      size = write(fh,&dataBuf[9],dataSize);
      if (size != dataSize)
      {
        logger(__FILE__,__LINE__,"IBrcvFile() => write() errno %d %s",errno,strerror(errno));

        rv = writeN(conn->sock,"96",2,IB_DEFAULT_TIMEOUT);
        if (rv != 0)
        {
          logger(__FILE__,__LINE__,"IBrcvFile() => writeN() error %d",rv);
          rv = IB_ERR_RCV_FILE_ERROR;
          break;
        }

        rv = IB_ERR_RCV_FILE_ERROR;
        break;
      }
    }
    rv = writeN(conn->sock,"00",2,IB_DEFAULT_TIMEOUT);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"IBrcvFile() => writeN() error %d",rv);
      rv = IB_ERR_RCV_FILE_ERROR;
      break;
    }
    if (dataSize == 0 || hasData != 'F')
    {
      g = 0;
    }
  }
  close(fh);
  return 0;
}

int IBrcvData(Connection *conn,char *rcvFile,void *data,
              int (* packetFunction)(void *data,char *buf,int buflen,int *use,int in_out),
              ...)
{
  int       rv,len=0,use=0;
  void      *data_;
  va_list   ap;
  char      rcvBuf[IB_MAXDATABLOCK + 1];
  int       (*packetFunction_)(void *data,char *buf,int buflen,int *use,int in_out);

  memset(rcvBuf,0,sizeof(rcvBuf));

  rv = RecvBlockData(conn,rcvBuf,&len);
  if(rv)
  {
    logger(__FILE__,__LINE__,"IBrcvData() => RecvBlockData error %d",rv);
    return rv;
  }
  
#ifdef _IB_DEBUG_
  logmsg_ex(__FILE__,__LINE__,"接受的数据:",rcvBuf,len);
#endif

  rv = (* packetFunction)(data,&rcvBuf[0],len,&use,PACKET_OUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBrcvData() => packetFunction error %d",rv);
    return IB_ERR_DASMRCVDATA_ERROR;
  }

  va_start(ap,packetFunction);

  while( ( data_ = va_arg(ap,void *) ) != NULL)
  {
    packetFunction_ = va_arg(ap,int *);
    rv = (* packetFunction_)(data_,&rcvBuf[0],len,&use,PACKET_OUT);
    if (rv != 0)
    {
      va_end(ap);
      logger(__FILE__,__LINE__,"IBrcvData() => packetFunction_ error %d",rv);
      return IB_ERR_DASMRCVDATA_ERROR;
    }
  }
  
  va_end(ap);

  if(conn->has_rcv_file)
  {
    if(rcvFile == NULL)
    {
      logger(__FILE__,__LINE__,"IBrcvData() => recv file name is null");
      return IB_ERR_RCVFILE_LOST;
    }

    rv = IBrcvFile(conn,rcvFile);
    if(rv)
    {
      logger(__FILE__,__LINE__,"IBrcvData() => IBrcvFile() error %d",rv);
      return rv;
    }
  }

  rv = SendBlockAck(conn);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBrcvData() => SendBlockAck() error %d",rv);
    return rv;
  }

  return 0;
}

int IBrcvDataEx(Connection *conn,char *rcvFile,void *data,
              int (* packetFunction)(void *data,char *buf,int buflen,int *use,int in_out),
              ...)
{
  int       rv,len=0,use=0;
  void      *data_;
  va_list   ap;
  char      rcvBuf[IB_MAXDATABLOCK + 1];
  int       (*packetFunction_)(void *data,char *buf,int buflen,int *use,int in_out);

  memset(rcvBuf,0,sizeof(rcvBuf));

  rv = RecvBlockDataEx(conn,rcvBuf,&len);
  if(rv)
  {
    logger(__FILE__,__LINE__,"IBrcvData() => RecvBlockData error %d",rv);
    return rv;
  }
  
#ifdef _IB_DEBUG_
  logmsg_ex(__FILE__,__LINE__,"接受的数据:",rcvBuf,len);
#endif

  rv = (* packetFunction)(data,&rcvBuf[0],len,&use,PACKET_OUT);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBrcvData() => packetFunction error %d",rv);
    return IB_ERR_DASMRCVDATA_ERROR;
  }

  va_start(ap,packetFunction);

  while( ( data_ = va_arg(ap,void *) ) != NULL)
  {
    packetFunction_ = va_arg(ap,int *);
    rv = (* packetFunction_)(data_,&rcvBuf[0],len,&use,PACKET_OUT);
    if (rv != 0)
    {
      va_end(ap);
      logger(__FILE__,__LINE__,"IBrcvData() => packetFunction_ error %d",rv);
      return IB_ERR_DASMRCVDATA_ERROR;
    }
  }
  
  va_end(ap);

  if(conn->has_rcv_file)
  {
    if(rcvFile == NULL)
    {
      logger(__FILE__,__LINE__,"IBrcvData() => recv file name is null");
      return IB_ERR_RCVFILE_LOST;
    }

    rv = IBrcvFileEx(conn,rcvFile);
    if(rv)
    {
      logger(__FILE__,__LINE__,"IBrcvData() => IBrcvFile() error %d",rv);
      return rv;
    }
  }

  rv = SendBlockAck(conn);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBrcvData() => SendBlockAck() error %d",rv);
    return rv;
  }

  return 0;
}

int IBsndData(Connection *conn,char *sndFile,void *data,
              int (* packetFunction)(void *data,char *buf,int buflen,int *use,int in_out),
              ...)
{
  int       rv,len=0,use=0;
  void      *data_;
  va_list   ap;
  char      sndBuf[IB_MAXDATABLOCK + 1];
  int       (*packetFunction_)(void *data,char *buf,int buflen,int *use,int in_out);

  memset(sndBuf,0,sizeof(sndBuf));

  if (sndFile != NULL)
  {
    conn->has_snd_file = 1;
  }
  else
  {
    conn->has_snd_file = 0;
  }

  rv = (* packetFunction)(  data,&sndBuf[0],IB_MAXDATABLOCK - 9,&use,PACKET_IN);
  if (rv != 0)
  {
    return IB_ERR_DASMRCVDATA_ERROR;
  }

  va_start(ap,packetFunction);

  while( ( data_ = va_arg(ap,void *) ) != NULL)
  {
    packetFunction_ = (int *)va_arg(ap,int *);
    rv = (* packetFunction_)(data_,&sndBuf[0],IB_MAXDATABLOCK - 9,&use,PACKET_IN);
    if (rv != 0)
    {
      va_end(ap);
      return IB_ERR_DASMRCVDATA_ERROR;
    }
  }

  va_end(ap);

  len = use;

  rv = SendBlockData(conn,sndBuf,len);

  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBsndData() => SendBlockData() error %d",rv);
    return rv;
  }

  if (conn->has_snd_file)
  {
    rv = IBsndFile(conn,sndFile);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"IBsndData() => SendBlockData() error %d",rv);
      return rv;
    }
  }

  rv = RecvBlockAck(conn);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBsndData() => RecvBlockAck() error %d",rv);
    return rv;
  }

  return 0;
}

int IBsndDataEx(Connection *conn,char *sndFile,void *data,
              int (* packetFunction)(void *data,char *buf,int buflen,int *use,int in_out),
              ...)
{
  int       rv,len=0,use=0;
  void      *data_;
  va_list   ap;
  char      sndBuf[IB_MAXDATABLOCK + 1];
  int       (*packetFunction_)(void *data,char *buf,int buflen,int *use,int in_out);

  memset(sndBuf,0,sizeof(sndBuf));

  if (sndFile != NULL)
  {
    conn->has_snd_file = 1;
  }
  else
  {
    conn->has_snd_file = 0;
  }

  rv = (* packetFunction)(  data,&sndBuf[0],IB_MAXDATABLOCK - 9,&use,PACKET_IN);
  if (rv != 0)
  {
    return IB_ERR_DASMRCVDATA_ERROR;
  }

  va_start(ap,packetFunction);

  while( ( data_ = va_arg(ap,void *) ) != NULL)
  {
    packetFunction_ = (int *)va_arg(ap,int *);
    rv = (* packetFunction_)(data_,&sndBuf[0],IB_MAXDATABLOCK - 9,&use,PACKET_IN);
    if (rv != 0)
    {
      va_end(ap);
      return IB_ERR_DASMRCVDATA_ERROR;
    }
  }

  va_end(ap);

  len = use;

  rv = SendBlockDataEx(conn,sndBuf,len);

  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBsndData() => SendBlockData() error %d",rv);
    return rv;
  }

  if (conn->has_snd_file)
  {
    rv = IBsndFileEx(conn,sndFile);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"IBsndData() => SendBlockData() error %d",rv);
      return rv;
    }
  }

  rv = RecvBlockAck(conn);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"IBsndData() => RecvBlockAck() error %d",rv);
    return rv;
  }

  return 0;
}

static int Mputenv(char *env)
{
  char    *p;

  p=(char *)malloc(strlen(env)+1);
  if (p==NULL)
    return -1;
  strcpy(p,env);
  return(putenv(p));
}

int IBsetenv(Connection *conn)
{
  char    env[128];

  sprintf(env,"%s=%d",ENV_SOCKN,conn->sock);
  Mputenv(env);
  sprintf(env,"%s=%s",ENV_NODE,conn->remote_node_id);
  Mputenv(env);
  return(0);
}

int IBgetenv(Connection *conn)
{
  char    *env;

  memset(conn,0,sizeof(Connection));
  env = (char *)getenv(ENV_SOCKN);
  if ( env == NULL )
  {
    logger(__FILE__,__LINE__,"IBgetenv() =>  getenv 发现环境\"%s\"丢失",ENV_SOCKN);
    return(IB_ERR_ENV_LOST);
  }
  conn->sock=atoi(env);
  
  env = (char *)getenv(ENV_NODE);
  if ( env == NULL )
  {
    logger(__FILE__,__LINE__,"IBgetenv() =>  getenv 发现环境\"%s\"丢失",ENV_NODE);
    return(IB_ERR_ENV_LOST);
  }
  strcpy(conn->remote_node_id,env);

  return 0;
}

int IBinitEnvEx()
{
  return 0;
}

int IBinitEnv()
{
  int  rv;

  signal(SIGPIPE,SIG_IGN);

  rv = IBsecInit();
  if (rv)
  {
    logger(__FILE__,__LINE__,"IBinitEnv() => IBsecInit() error %d",rv);
    return IB_ERR_INIT_ERROR;
  }

  return 0;
}

int IBacceptConnect(Connection *conn)
{
  int  rv;

  rv = IBinitEnv();
  if(rv)
  {
    logger(__FILE__,__LINE__,"IBacceptConnect() => IBinitEnv() error %d",rv);
    return rv;
  }

  rv = IBgetenv(conn);
  if(rv) 
  {
    logger(__FILE__,__LINE__,"IBacceptConnect() => IBgetenv() error %d",rv);
    return rv;
  }

  return 0;
}

int IBacceptConnectEx(Connection *conn)
{
  int  rv;

  rv = IBinitEnvEx();
  if(rv)
  {
    logger(__FILE__,__LINE__,"IBacceptConnect() => IBinitEnv() error %d",rv);
    return rv;
  }

  rv = IBgetenv(conn);
  if(rv) 
  {
    logger(__FILE__,__LINE__,"IBacceptConnect() => IBgetenv() error %d",rv);
    return rv;
  }

  return 0;
}

int IBdisconnect(Connection *conn)
{
  int   rv;

  if(conn->sock >= 0)
  {
    close(conn->sock);
    conn->sock = -1;
  }

  return 0;
}

static int setSockAddr(struct sockaddr_in *addr, char *hostname, int portNo)
{
  struct hostent  *host;

  host = gethostbyname(hostname);
  if (host == NULL)
  {
    logger(__FILE__,__LINE__,"setSockAddr() => gethostbyname() errno %d,%s",errno,strerror(errno));
    return IB_ERR_NOT_FIND_HOSTNAME;
  }

  memset(addr,0,sizeof(struct sockaddr_in));
  memmove((char *)&addr->sin_addr,host->h_addr,host->h_length);
  addr->sin_port = htons(portNo);
  addr->sin_family = AF_INET;
  return 0;
}


static int InterSockOpen(Connection *conn, char *node)
{
  int                 sock;
  struct sockaddr_in  addr;
  char                addrStat[4];
  char                addr1[32];
  char                addr2[32];
  int                 portNo;
  int                 rv;
  extern   int        errno;

  rv = IBGetAddr(node,addrStat,addr1,addr2,&portNo);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"Get Addr ERR[%d][%s]",rv,node);
    return IB_ERR_GET_ADDR_ERROR;
  }

  rv = setSockAddr(&addr,addr1,portNo);
  if (rv != 0)
  {
    return rv;
  }


  sock = socket ( AF_INET, SOCK_STREAM, 0 );
  if ( sock < 0 )
  {
    logger(__FILE__,__LINE__,"open socket error %d,%s",errno,strerror(errno));
     return IB_ERR_SOCKET_FAIL;
  }
  
  if ( connect ( sock, ( struct sockaddr * ) &addr, sizeof addr ) < 0 ) 
  {
    logger ( __FILE__, __LINE__ ,"Connect priamry server failed!,errno=%d,%s", 
                                  errno,strerror(errno));
    close ( sock );
  }
  else
  {
    conn->sock = sock;
    return 0;
  }

  rv = setSockAddr(&addr,addr1,portNo);
  if (rv != 0)
  {
    return rv;
  }

  sock = socket ( AF_INET, SOCK_STREAM, 0 );
  if ( sock < 0 )
  {
    logger(__FILE__,__LINE__,"open socket error %d,%s",errno,strerror(errno));
    return IB_ERR_SOCKET_FAIL;
  }

  if ( connect ( sock, ( struct sockaddr * ) &addr, sizeof addr ) < 0 )
  {
    logger ( __FILE__, __LINE__, "Connect priamry and backup server failed!,errno=%d",
                                 errno ,strerror(errno));
    close ( sock );

    return IB_ERR_CONNECT_FAIL;
  }
  else 
  {
    addrStat[0] = (addrStat[0] == 'B') ? 'P' : 'B';
    IBSetAddrStat(node,addrStat);
    conn->sock = sock;
    return 0;
  }
}

static int _connect_(Connection *conn,char *msgType,char *node,char *tranCode)
{
  char  local_node_id[NODE_MAXLEN + 1];
  int   rv;
 
  memset(conn,0,sizeof(Connection));
  conn->sock = -1;

  rv = IBsecGetSecNode(local_node_id,NODE_MAXLEN);
  if(rv)
  {
    logger(__FILE__,__LINE__,"_connect_() => IBsecGetSecNode() error %d ",rv);
    return IB_ERR_NOT_SET_NODE_ID;
  }

  strncpy(conn->message_type,msgType,MSG_TYPE_LEN);
  strncpy(conn->remote_node_id,node,NODE_MAXLEN);
  strncpy(conn->local_node_id,local_node_id,NODE_MAXLEN);
  strncpy(conn->trans_code,tranCode,TRANS_CODE_LEN);

  rv = InterSockOpen(conn, node);
  if( rv ) 
  {
    return rv;
  }

  return 0;
}

static int _IBconnect_(Connection *conn,char *node,char *tranCode,char *msgType)
{
  int      rv,response = 0;
  int      count;
  int      transTimeout;
  int      acktransTimeout;
  time_t   startTime,currentTime;

  memset(conn, 0, sizeof(Connection));
  conn->sock = -1;

  rv = IBGetTransTimeout(msgType,tranCode,&transTimeout);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"[%s] [%s],GetTransTimeOut ERR", msgType,tranCode);
    rv = IB_ERR_GETTRANSTIMEOUT_ERROR;
    return rv;
  }

  IB_TRANS_TIMEOUT = transTimeout;

  rv = _connect_(conn,msgType,node,tranCode);
  if ( rv != 0 )
  {
    logger(__FILE__,__LINE__," connect err=%d",rv);
    return rv;
  }

  rv = SendHeader(conn);
  if (rv != 0)
  {
    goto E;
  }

  rv = RecvHeaderAck(conn,&response);
  if(rv != 0 || response != 0)
  {
    rv = -1;
    goto E;
  }

E:
  return rv; 
}


int IBconnect(Connection *conn,void *pubdata,int (*get_pub_data_f)(void *pubdata,
              char *node,char *tranCode,char *msgType))
{
  int      rv;
  char     node[NODE_MAXLEN+1];
  char     msgType[MSG_TYPE_LEN+1];
  char     tranCode[TRANS_CODE_LEN+1];

  rv = IBinitEnv();
  if (rv != 0)
  {
    return(IB_ERR_INIT_ERROR);
  }

  memset(conn, 0, sizeof(Connection));
  memset(node,0,sizeof(node));
  memset(tranCode,0,sizeof(tranCode));
  memset(msgType,0,sizeof(msgType));
  rv = (*get_pub_data_f)(pubdata,msgType,node,tranCode);
  if (rv != 0)
  {
    return IB_ERR_CONNECT_ERROR;
  }

  rv = _IBconnect_(conn,node,tranCode,msgType);

  return rv;  
}

int IBconnectEx(Connection *conn,void *pubdata,int (*get_pub_data_f)(void *pubdata,
              char *node,char *tranCode,char *msgType))
{
  int      rv;
  char     node[NODE_MAXLEN+1];
  char     msgType[MSG_TYPE_LEN+1];
  char     tranCode[TRANS_CODE_LEN+1];

  rv = IBinitEnvEx();
  if (rv != 0)
  {
    return(IB_ERR_INIT_ERROR);
  }

  memset(conn, 0, sizeof(Connection));
  memset(node,0,sizeof(node));
  memset(tranCode,0,sizeof(tranCode));
  memset(msgType,0,sizeof(msgType));
  rv = (*get_pub_data_f)(pubdata,msgType,node,tranCode);
  if (rv != 0)
  {
    return IB_ERR_CONNECT_ERROR;
  }

  rv = _IBconnect_(conn,node,tranCode,msgType);

  return rv;  
}


