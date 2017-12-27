#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include "sockets.h"

struct TSocket
{
  int    socketdes;
  struct sockaddr_in  addr;
};

int SocketClose(TSocket **sock)
{
  if ( (*sock) != NULL )
  {
    if ( (*sock)->socketdes > 0)
    {
      close( (*sock)->socketdes );
    }
    (*sock)->socketdes = -1;
    free(*sock) ;
    *sock = NULL;
  }
  return TTS_SUCCESS;
}

int SocketShutdown(TSocket *thesocket, TSocketShutdownHow how)
{
  return (shutdown(thesocket->socketdes, how) == -1) ? errno : TTS_SUCCESS;
}

int SocketServer(TSocket **sock,const char *addr,int port)
{
  TSocket   *new;
  int        optval;
  int        rv;
  
  new = (TSocket *)malloc( sizeof(TSocket) );
  if ( new == NULL )
  {
    return TTS_ENULL;
  }
  
  memset(new,0,sizeof(TSocket));

  new->socketdes = socket( AF_INET, SOCK_STREAM, 0 );
  if ( new->socketdes < 0 )
  {
    SocketClose(&new);
    return errno;
  }

  memset(&(new->addr),0,sizeof(new->addr));

  if ( addr != NULL && strlen(addr) > 7 )
  {
    new->addr.sin_addr.s_addr = inet_addr(addr);
  }
  else
  {
    new->addr.sin_addr.s_addr = htonl ( INADDR_ANY );
  }

  new->addr.sin_port = htons(port);
  new->addr.sin_family = AF_INET;
  
  optval = 1;

#ifdef SO_REUSEADDR
  if ( setsockopt( new->socketdes, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int) ) < 0 )
  {
    SocketClose(&new);
    return errno;
  }
#endif

#ifdef SO_REUSEPORT
  if ( setsockopt( new->socketdes, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int) ) < 0 )
  {
    SocketClose(&new);
    return errno;
  }
#endif

  if ( bind( new->socketdes, (struct sockaddr *)&(new->addr), sizeof(new->addr) ) == -1 )
  {
    SocketClose(&new);
    return errno;
  }

  if (listen(new->socketdes, 15) == -1)
  {
    SocketClose(&new);
    return errno;
  }

  *sock = new ;

  return TTS_SUCCESS;
}

int SocketAccept(TSocket **sock, TSocket *server)
{
  int                 s,salen,rv;
  struct sockaddr_in  sa;
  TSocket            *new;

  salen = sizeof(sa);
  s = accept(server->socketdes, (struct sockaddr *)&sa, &salen);
  if ( s < 0 )
  {
    return errno;
  }
  
  new = (TSocket *)malloc(sizeof(TSocket));
  memset(new,0,sizeof(TSocket));
  new->socketdes = s;

  memcpy(&(new->addr),&sa,salen);
 
  *sock = new ;

  return TTS_SUCCESS;
}

int SocketAcceptEx(TSocket **sock, TSocket *server,int timeout)
{
  int                 s,salen,rv;
  struct sockaddr_in  sa;
  TSocket            *new;
  fd_set              rset;
  struct timeval      tv;

  FD_ZERO(&rset);
  FD_SET(server->socketdes, &rset);
  tv.tv_sec = timeout;
  tv.tv_usec= 0;

  salen = sizeof(sa);

  rv = select(server->socketdes + 1,&rset,NULL,NULL, timeout > 0 ? &tv : NULL);
  if ( rv == 0 )
  {
    return TTS_TIMEOUT;
  }
  else if ( rv < 0 )
  {
    return errno;
  }

  s = accept(server->socketdes, (struct sockaddr *)&sa, &salen);
  if ( s < 0 )
  {
    return errno;
  }
  
  new = (TSocket *)malloc(sizeof(TSocket));
  memset(new,0,sizeof(TSocket));
  new->socketdes = s;

  memcpy(&(new->addr),&sa,salen);
 
  *sock = new ;

  return TTS_SUCCESS;
}


int SocketConnect(TSocket **sock,const char *addr,int port)
{
  TSocket   *new;
  int        optval;
  int   rv;

  if ( addr == NULL || strlen(addr) < 7 || port < 1)
  {
    return TTS_EINVAL;
  }

  new = (TSocket *)malloc( sizeof(TSocket) );
  if ( new == NULL )
  {
    return TTS_ENULL;
  }
  
  memset(new,0,sizeof(TSocket));

  new->socketdes = socket( AF_INET, SOCK_STREAM, 0 );
  if ( new->socketdes < 0 )
  {
    SocketClose(&new);
    return errno;
  }
  memset(&(new->addr),0,sizeof(new->addr));
  new->addr.sin_addr.s_addr = inet_addr(addr);
  new->addr.sin_port = htons(port);
  new->addr.sin_family = AF_INET;

  if ( ( connect(new->socketdes,(const struct sockaddr *)&(new->addr),sizeof(struct sockaddr_in)) ) < 0 )
  {
    SocketClose(&new);
    return errno;
  }

  *sock = new;
  
  return TTS_SUCCESS;
}

int SocketConnectEx(TSocket **sock,const char *addr,int port,int timeout)
{
  TSocket        *new;
  int             found,flags;
  fd_set          rset, wset;
  struct timeval  tt;
  int             rv;
  int             error = -1,len;

  if ( addr == NULL || strlen(addr) < 7 || port < 1 || timeout < 0)
  {
    return TTS_EINVAL;
  }

  new = (TSocket *)malloc( sizeof(TSocket) );
  if ( new == NULL )
  {
    return TTS_ENULL;
  }
  
  memset(new,0,sizeof(TSocket));

  new->socketdes = socket( AF_INET, SOCK_STREAM, 0 );
  if ( new->socketdes < 0 )
  {
    SocketClose(&new);
    return errno;
  }

  memset(&(new->addr),0,sizeof(new->addr));
  new->addr.sin_addr.s_addr = inet_addr(addr);
  new->addr.sin_port = htons(port);
  new->addr.sin_family = AF_INET;

  if(( flags = fcntl((new->socketdes), F_GETFL, 0) ) == -1 ||
        fcntl((new->socketdes), F_SETFL, flags | O_NONBLOCK ) == -1 )
  {
    SocketClose(&new);
    return errno;
  }

  if ( connect((new->socketdes),(const struct sockaddr *)&(new->addr),sizeof(new->addr)) < 0)
  {
    if ( errno != EINPROGRESS )
    {
      SocketClose(&new);
      return errno;
    }
    errno = 0;
  }
  else
  {
    goto CONNECT_OK;
  }


  FD_ZERO(&rset);
  FD_SET( new->socketdes, &rset);
  wset = rset;
  
  tt.tv_sec = timeout;
  tt.tv_usec = 0;
  
  found = select( (new->socketdes) + 1, &rset, &wset, NULL, (timeout > 0) ? &tt : NULL);
  if( found < 0 )
  {
    SocketClose(&new);
    return errno;
  }
  else if ( found == 0 )
  {
    SocketClose(&new);
    return TTS_TIMEOUT;
  }

  if ( getsockopt(new->socketdes, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0 )
  {
    SocketClose(&new);
    return errno;
  }

  if( error == 0 && (FD_ISSET( new->socketdes, &rset) || FD_ISSET( new->socketdes, &wset) ))
  {
    goto CONNECT_OK;
  }
  else
  {
    goto CONNECT_ERROR;
  }

CONNECT_OK:
  
  if(fcntl(new->socketdes, F_SETFL, flags) == -1 )
  {
    goto CONNECT_ERROR;
  }


  *sock = new;

  return TTS_SUCCESS;

CONNECT_ERROR:

  SocketClose(&new);

  return errno;
}

static int readn(int fd,char *buffer,int len,int timeout)
{
  int             n = 0;
  fd_set          readfds;
  int             found;
  long            startTime,currentTime;
  struct timeval  tt;
  
  FD_ZERO(&readfds);
  time(&startTime);

  while ( len > 0 )
  {
    time(&currentTime);
    if( timeout > 0 && (currentTime - startTime) > timeout)
    {
      return TTS_TIMEOUT;
    }
    else
    {
      FD_SET(fd,&readfds);
      tt.tv_sec = timeout - (currentTime - startTime);
      tt.tv_usec = 0;

      found = select(fd + 1,&readfds,NULL,NULL,timeout > 0 ? &tt : NULL );
      if (found == 0)
      {
        return TTS_TIMEOUT;
      }
      else if (found < 0)
      {
        return errno;
      }
      else
      {
        n = read(fd,buffer,len);
        if (n <= 0)
        {
          return errno;
        }
        else
        {
          len -= n;
          buffer += n;
        }
      }
    }
  }

  return TTS_SUCCESS;
}

static int readnolen(int fd,char *buffer,int *len,int timeout)
{
  int             n = 0;
  fd_set          readfds;
  int             found;
  long            startTime,currentTime;
  struct timeval  tt;
  char            *ptr;
  
  FD_ZERO(&readfds);
  time(&startTime);
  
  ptr = buffer;
  while ( 1 )
  {
    time(&currentTime);
    if( timeout > 0 && (currentTime - startTime) > timeout)
    {
      return TTS_TIMEOUT;
    }
    else
    {
      FD_SET(fd,&readfds);
      tt.tv_sec = timeout - (currentTime - startTime);
      tt.tv_usec = 0;

      found = select(fd + 1,&readfds,NULL,NULL,timeout > 0 ? &tt : NULL );
      if (found == 0)
      {
        return TTS_TIMEOUT;
      }
      else if (found < 0)
      {
        return errno;
      }
      else
      {
        n = read(fd,ptr,1);
        if (n < 0)
        {
          return errno;
        }
        else if (n == 0)
        {
          break;
        }
        else
        {
          *len += 1;
          ptr += 1;
        }
      }
    }
  }

  return TTS_SUCCESS;
}

static int writen(int fd,char *buffer,int len,int timeout)
{
  int             n = 0;
  fd_set          writefds;
  int             found;
  long            startTime,currentTime;
  struct timeval  tt;

  FD_ZERO(&writefds);
  time(&startTime);

  while ( len > 0 )
  {
    time(&currentTime);
    if(timeout > 0 && (currentTime - startTime) > timeout)
    {
      return TTS_TIMEOUT;
    }
    else
    {
      FD_SET(fd,&writefds);
      tt.tv_sec = timeout - (currentTime - startTime);
      tt.tv_usec = 0;
      found = select(fd + 1,NULL,&writefds,NULL,timeout > 0 ? &tt : NULL);
      if (found == 0)
      {
        return TTS_TIMEOUT;
      }
      else if (found < 0)
      {
        return errno;
      }
      else
      {
        n = write(fd,buffer,len);
        if (n <= 0)
        {
          return errno;
        }
        else
        {
          len -= n;
          buffer += n;
        }
      }
    }
  }

  return TTS_SUCCESS;
}

int SocketWrite(TSocket *sock,char *buf,int len,int timeout)
{
  if  ( sock == NULL )
  {
    return TTS_EINVAL;
  }

  return writen(sock->socketdes,buf,len,timeout);
}

int SocketRead(TSocket *sock,char *buf,int len,int timeout)
{
  if ( sock == NULL )
  {
    return TTS_EINVAL;
  }
  return readn(sock->socketdes,buf,len,timeout);
}

int SocketRcv(TSocket *sock,char *buf,int *len,int timeout)
{
  if ( sock == NULL )
  {
    return TTS_EINVAL;
  }
  
  return readnolen(sock->socketdes,buf,len,timeout);
}

int SocketRecv(TSocket *sock,char *buf,int len)
{
  int   n ;

  while ( len > 0 )
  {
    n = read(sock->socketdes,buf,len);
    if ( n < 0 )
    {
      return errno;
    }
    else if ( n == 0 )
    {
      break;
    }
    len -= n;
    buf += n;
  }
  
  return 0;
}

int SocketWriteEx(TSocket *sock,char *buf,int len,int timeout,int flag)
{
  char      dlen[5];
  int       wlen;
  int  rv;

  if ( sock == NULL )
  {
    return TTS_EINVAL;
  }

  memset(dlen,0,sizeof(dlen));

  if ( flag == SOCKET_ASC_LENGTH)
  {
    sprintf(dlen,"%04d",len);
    wlen = 4;
  }
  else if ( flag == SOCKET_HEX_LENGTH )
  {
    dlen[0] = (len / 0xFF) & 0xFF;
    dlen[1] = (len % 0xFF) & 0xFF;
    wlen = 2;
  }
  else 
  {
    return TTS_EINVAL;
  }
  
  rv = writen(sock->socketdes,dlen,wlen,timeout);
  if (rv == TTS_SUCCESS)
  {
    rv = writen(sock->socketdes,buf,len,timeout);
  }

  return rv;

}

int SocketReadEx(TSocket *sock,char *buf,int *len,int timeout,int flag)
{
  char      dlen[5];
  int       rlen;
  int  rv;

  if ( sock == NULL )
  {
    return TTS_EINVAL;
  }

  memset(dlen,0,sizeof(dlen));

  if ( flag == SOCKET_ASC_LENGTH )
  {
    rlen = 4;
  }
  else if ( flag == SOCKET_HEX_LENGTH )
  {
    rlen = 2;
  }
  else
  {
    return TTS_EINVAL;
  }

  rv = readn(sock->socketdes,dlen,rlen,timeout);
  if (rv != TTS_SUCCESS)
  {
    return rv;
  }

  memcpy(buf,dlen,rlen);

  if ( flag == SOCKET_ASC_LENGTH )
  {
    *len = atoi(dlen);
  }
  else if ( flag == SOCKET_HEX_LENGTH )
  {
    *len = (int)(dlen[0] & 0xFF) * 0xFF + (int)(dlen[1] & 0xFF) ;
  }

  if (  (*len)  <= 0 )
  {
    return TTS_EINVAL;
  }

  rv = readn(sock->socketdes,buf + rlen,*len,timeout);
  
  if ( rv == TTS_SUCCESS )
  {
    *len += rlen;
  }

  return rv; 

}

int SocketGetHandle(const TSocket *sock)
{
  if ( sock == NULL )
  {
    return TTS_EINVAL;
  }

  return sock->socketdes;
}

int SocketSetHandle(TSocket *sock,int hd)
{
  sock->socketdes = hd;
  return TTS_SUCCESS;
}

int SocketGetAddress(TSocket *sock,char *address)
{
  struct sockaddr  name;
  socklen_t        namelen;

  namelen = sizeof(name);
  memset(&name,0,namelen);

  if ( sock == NULL )
  {
    return TTS_ENULL;
  }
 
  if ( getpeername(sock->socketdes,&name,&namelen) < 0 )
  {
    return errno;
  } 

  if ( name.sa_family == AF_INET )
  {
    if ( address )
    {
      strcpy( address,(char *)inet_ntoa( ((struct sockaddr_in *)&name)->sin_addr ) );
    }
  }
  else
  {
    return TTS_ENULL;
  }
  return TTS_SUCCESS;
}


int SocketGetProtocolAddress(TSocket *sock, char *local, char *remote)
{
  struct sockaddr  name;
  socklen_t        namelen;

  namelen = sizeof(name);
  memset(&name,0,namelen);

  if ( getpeername(sock->socketdes,&name,&namelen) < 0 )
  {
    return errno;
  }

  if ( name.sa_family == AF_INET )
  {
    snprintf(remote, 23, "%s:%d",inet_ntoa( ((struct sockaddr_in *)&name)->sin_addr ),
                                 ntohs( ((struct sockaddr_in *)&name)->sin_port ) );
  }
  else
  {
    return TTS_ENULL;
  }

  namelen = sizeof(name);
  memset(&name,0,namelen);

  if ( getsockname(sock->socketdes,&name,&namelen) < 0 )
  {
    return errno;
  }

  if ( name.sa_family == AF_INET )
  {
    snprintf(local, 23, "%s:%d",inet_ntoa( ((struct sockaddr_in *)&name)->sin_addr ),
                                ntohs( ((struct sockaddr_in *)&name)->sin_port ) );
  }
  else
  {
    return TTS_ENULL;
  }
  return TTS_SUCCESS;
}

int UDPSocketServer(TSocket **sock,const char *addr,int port)
{
  TSocket          *new;
  int               optval;
  int               rv;
  struct protoent  *protos;
  
  protos = getprotobyname("udp");
  if ( protos = NULL )
  {
     return TTS_EINVAL;
  }

  new = (TSocket *)malloc( sizeof(TSocket) );
  if ( new == NULL )
  {
    return TTS_ENULL;
  }
  
  memset(new,0,sizeof(TSocket));

  
  new->socketdes = socket( AF_INET, SOCK_DGRAM, protos->p_proto );
  if ( new->socketdes < 0 )
  {
    SocketClose(&new);
    return errno;
  }

  memset(&(new->addr),0,sizeof(new->addr));

  if ( addr != NULL && strlen(addr) > 7 )
  {
    new->addr.sin_addr.s_addr = inet_addr(addr);
  }
  else
  {
    new->addr.sin_addr.s_addr = htonl ( INADDR_ANY );
  }

  new->addr.sin_port = htons(port);
  new->addr.sin_family = AF_INET;
  
  optval = 1;

#ifdef SO_REUSEADDR
  if ( setsockopt( new->socketdes, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int) ) < 0 )
  {
    SocketClose(&new);
    return errno;
  }
#endif

#ifdef SO_REUSEPORT
  if ( setsockopt( new->socketdes, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int) ) < 0 )
  {
    SocketClose(&new);
    return errno;
  }
#endif

  if ( bind( new->socketdes, (struct sockaddr *)&(new->addr), sizeof(new->addr) ) == -1 )
  {
    SocketClose(&new);
    return errno;
  }

  *sock = new ;

  return TTS_SUCCESS;
}

