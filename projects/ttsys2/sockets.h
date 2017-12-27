
/**
 * @file sockets.h
 * @brief TCP ���������
 * @author tomszhou
 * @version 1.0.1.0
 * @date 2008-4-12
 */


#ifndef _TTSYS_SOCKETS_H_
#define _TTSYS_SOCKETS_H_



#include "ttdef.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_SOCKETS TCP �������
 * @ingroup TTSYS
 * @{
 */

/**
 * TCP �����������
 */
typedef struct TSocket   TSocket;

/**
 * ���紫�䳤�����Ͷ���
 */
enum
{
  SOCKET_NONE_LENGTH = 0,       /**< ��������ĩ���� */
  SOCKET_ASC_LENGTH  = 1,       /**< ��������ΪASC�� */
  SOCKET_HEX_LENGTH  = 2        /**< ��������ΪHEX�� */
};

/**
 * ������ιرն���
 */
typedef enum
{
  SOCKET_SHUTDOWN_READ,         /**< �رն����� */
  SOCKET_SHUTDOWN_WRITE,        /**< �ر�д���� */
  SOCKET_SHUTDOWN_READWRITE     /**< �رն�д����*/
} TSocketShutdownHow;


/** �رղ������������
 *  @param sock SOCKET���
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketClose(TSocket **sock);

/** �ر������������
 *  @param thesocket  SOCKET���
 *  @param how  Ҫ�رյĹ���
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketShutdown(TSocket *thesocket, TSocketShutdownHow how);

/** �����������(TCP)
 *  @param sock SOCKET���
 *  @param addr ��ַ
 *  @param port �˿�
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketServer(TSocket **sock,const char *addr,int port);

/** ����SOCKET�������
 *  @param new SOCKET���
 *  @param sock SOCKET�������
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketAccept(TSocket **new, TSocket *sock);

/** ����SOCKET�������
 *  @param new SOCKET���
 *  @param sock SOCKET�������
 *  @param timeout �ڹ涨��ʱ���ڵȴ�����(timeout=0 ʱû��ʱ������)
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketAcceptEx(TSocket **new, TSocket *sock,int timeout);

/** ���ӶԷ�����
 *  @param new SOCKET���
 *  @param addr ��ַ
 *  @param port �˿�
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketConnect(TSocket **new,const char *addr,int port);

/** �ڹ涨��ʱ�������ӶԷ�����
 *  @param new SOCKET���
 *  @param addr ��ַ
 *  @param port �˿�
 *  @param timeout ��ʱʱ��
 *  @return ���� TTS_SUCCESS �ɹ�
 */
int SocketConnectEx(TSocket **new,const char *addr,int port,int timeout);

/** ͨ��SOCKET�����������
 *  @param[in] sock SOCKET���
 *  @param[in] buf ����
 *  @param[in] len ����
 *  @param[in] timeout ��ʱʱ��
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketWrite(TSocket *sock,char *buf,int len,int timeout);

/** ͨ��SOCKET�����������
 *  @param[in] sock SOCKET���
 *  @param[out] buf ����
 *  @param[in] len ����
 *  @param[in] timeout ��ʱʱ��
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketRead(TSocket *sock,char *buf,int len,int timeout);

/** ͨ��SOCKET�����������(δ֪����)
 *  @param[in] sock SOCKET���
 *  @param[out] buf ����
 *  @param[out] len ����
 *  @param[in] timeout ��ʱʱ��
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketRcv(TSocket *sock,char *buf,int *len,int timeout);

/** ͨ��SOCKET�����������(�ȴ�)
 *  @param[in] sock SOCKET���
 *  @param[out] buf ����
 *  @param[out] len ����
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketRecv(TSocket *sock,char *buf,int len);


/** ͨ��SOCKET�����������(�Զ�������)
 *  @param[in] sock SOCKET���
 *  @param[in] buf ����
 *  @param[in] len ����
 *  @param[in] timeout ��ʱʱ��
 *  @param[in] flag  �������ͱ�ʶ
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketWriteEx(TSocket *sock,char *buf,int len,int timeout,int flag);

/** ͨ��SOCKET�����������(�Զ�������)
 *  @param[in] sock SOCKET���
 *  @param[out] buf ����
 *  @param[out] len ����
 *  @param[in] timeout ��ʱʱ��
 *  @param[in] flag  �������ͱ�ʶ
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketReadEx(TSocket *sock,char *buf,int *len,int timeout,int flag);

/** ��ȡSOCKET����ľ��
 *  @param[in] sock SOCKET���
 *  @return SOCKET�ľ��
 */
int SocketGetHandle(const TSocket *sock);

/** ����SOCKET����ľ��
 *  @param[in] sock SOCKET���
 *  @param[in] hd SOCKET�ľ��
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketSetHandle(TSocket *sock,int hd);


/** ��ȡSOCKET�����IP��ַ
 *  @param[in] sock SOCKET���
 *  @param[out] address �Է�IP��ַ
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketGetAddress(TSocket *sock,char *address);

/** ��ȡSOCKET�����Э��IP��ַ�Ͷ˿�
 *  @param[in] sock SOCKET���
 *  @param[out] local ����IP��ַ�Ͷ˿�
 *  @param[out] remote �Է�IP��ַ�Ͷ˿�
 *  @return ����TTS_SUCCESS�ɹ�
 */
int SocketGetProtocolAddress(TSocket *sock, char *local, char *remote);

/** �����������(UDP)
 *  @param sock SOCKET���
 *  @param addr ��ַ
 *  @param port �˿�
 *  @return ����TTS_SUCCESS�ɹ�
 */
int UDPSocketServer(TSocket **sock,const char *addr,int port);
/** @} */

__END_DECLS

#endif



