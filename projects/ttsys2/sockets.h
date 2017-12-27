
/**
 * @file sockets.h
 * @brief TCP 网络组件。
 * @author tomszhou
 * @version 1.0.1.0
 * @date 2008-4-12
 */


#ifndef _TTSYS_SOCKETS_H_
#define _TTSYS_SOCKETS_H_



#include "ttdef.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_SOCKETS TCP 网络组件
 * @ingroup TTSYS
 * @{
 */

/**
 * TCP 网络组件定义
 */
typedef struct TSocket   TSocket;

/**
 * 网络传输长度类型定义
 */
enum
{
  SOCKET_NONE_LENGTH = 0,       /**< 长度类型末定义 */
  SOCKET_ASC_LENGTH  = 1,       /**< 长度类型为ASC码 */
  SOCKET_HEX_LENGTH  = 2        /**< 长度类型为HEX码 */
};

/**
 * 网络如何关闭定义
 */
typedef enum
{
  SOCKET_SHUTDOWN_READ,         /**< 关闭读功能 */
  SOCKET_SHUTDOWN_WRITE,        /**< 关闭写功能 */
  SOCKET_SHUTDOWN_READWRITE     /**< 关闭读写功能*/
} TSocketShutdownHow;


/** 关闭并销毁网络组件
 *  @param sock SOCKET组件
 *  @return 返回TTS_SUCCESS成功
 */
int SocketClose(TSocket **sock);

/** 关闭网络组件功能
 *  @param thesocket  SOCKET组件
 *  @param how  要关闭的功能
 *  @return 返回TTS_SUCCESS成功
 */
int SocketShutdown(TSocket *thesocket, TSocketShutdownHow how);

/** 创建网络服务(TCP)
 *  @param sock SOCKET组件
 *  @param addr 地址
 *  @param port 端口
 *  @return 返回TTS_SUCCESS成功
 */
int SocketServer(TSocket **sock,const char *addr,int port);

/** 根据SOCKET组件侦听
 *  @param new SOCKET组件
 *  @param sock SOCKET服务组件
 *  @return 返回TTS_SUCCESS成功
 */
int SocketAccept(TSocket **new, TSocket *sock);

/** 根据SOCKET组件侦听
 *  @param new SOCKET组件
 *  @param sock SOCKET服务组件
 *  @param timeout 在规定的时间内等待连接(timeout=0 时没有时间限制)
 *  @return 返回TTS_SUCCESS成功
 */
int SocketAcceptEx(TSocket **new, TSocket *sock,int timeout);

/** 链接对方主机
 *  @param new SOCKET组件
 *  @param addr 地址
 *  @param port 端口
 *  @return 返回TTS_SUCCESS成功
 */
int SocketConnect(TSocket **new,const char *addr,int port);

/** 在规定的时间内链接对方主机
 *  @param new SOCKET组件
 *  @param addr 地址
 *  @param port 端口
 *  @param timeout 超时时间
 *  @return 返回 TTS_SUCCESS 成功
 */
int SocketConnectEx(TSocket **new,const char *addr,int port,int timeout);

/** 通过SOCKET组件发送数据
 *  @param[in] sock SOCKET组件
 *  @param[in] buf 数据
 *  @param[in] len 长度
 *  @param[in] timeout 超时时间
 *  @return 返回TTS_SUCCESS成功
 */
int SocketWrite(TSocket *sock,char *buf,int len,int timeout);

/** 通过SOCKET组件接受数据
 *  @param[in] sock SOCKET组件
 *  @param[out] buf 数据
 *  @param[in] len 长度
 *  @param[in] timeout 超时时间
 *  @return 返回TTS_SUCCESS成功
 */
int SocketRead(TSocket *sock,char *buf,int len,int timeout);

/** 通过SOCKET组件接受数据(未知长度)
 *  @param[in] sock SOCKET组件
 *  @param[out] buf 数据
 *  @param[out] len 长度
 *  @param[in] timeout 超时时间
 *  @return 返回TTS_SUCCESS成功
 */
int SocketRcv(TSocket *sock,char *buf,int *len,int timeout);

/** 通过SOCKET组件接受数据(等待)
 *  @param[in] sock SOCKET组件
 *  @param[out] buf 数据
 *  @param[out] len 长度
 *  @return 返回TTS_SUCCESS成功
 */
int SocketRecv(TSocket *sock,char *buf,int len);


/** 通过SOCKET组件发送数据(自动带长度)
 *  @param[in] sock SOCKET组件
 *  @param[in] buf 数据
 *  @param[in] len 长度
 *  @param[in] timeout 超时时间
 *  @param[in] flag  长度类型标识
 *  @return 返回TTS_SUCCESS成功
 */
int SocketWriteEx(TSocket *sock,char *buf,int len,int timeout,int flag);

/** 通过SOCKET组件接受数据(自动带长度)
 *  @param[in] sock SOCKET组件
 *  @param[out] buf 数据
 *  @param[out] len 长度
 *  @param[in] timeout 超时时间
 *  @param[in] flag  长度类型标识
 *  @return 返回TTS_SUCCESS成功
 */
int SocketReadEx(TSocket *sock,char *buf,int *len,int timeout,int flag);

/** 获取SOCKET组件的句柄
 *  @param[in] sock SOCKET组件
 *  @return SOCKET的句柄
 */
int SocketGetHandle(const TSocket *sock);

/** 设置SOCKET组件的句柄
 *  @param[in] sock SOCKET组件
 *  @param[in] hd SOCKET的句柄
 *  @return 返回TTS_SUCCESS成功
 */
int SocketSetHandle(TSocket *sock,int hd);


/** 获取SOCKET组件的IP地址
 *  @param[in] sock SOCKET组件
 *  @param[out] address 对方IP地址
 *  @return 返回TTS_SUCCESS成功
 */
int SocketGetAddress(TSocket *sock,char *address);

/** 获取SOCKET组件的协议IP地址和端口
 *  @param[in] sock SOCKET组件
 *  @param[out] local 本地IP地址和端口
 *  @param[out] remote 对方IP地址和端口
 *  @return 返回TTS_SUCCESS成功
 */
int SocketGetProtocolAddress(TSocket *sock, char *local, char *remote);

/** 创建网络服务(UDP)
 *  @param sock SOCKET组件
 *  @param addr 地址
 *  @param port 端口
 *  @return 返回TTS_SUCCESS成功
 */
int UDPSocketServer(TSocket **sock,const char *addr,int port);
/** @} */

__END_DECLS

#endif



