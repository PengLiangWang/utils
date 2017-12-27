
/**
 * @file msgqueue.h
 * @brief 系统消息队列组件，对 SYSTEM MESSAGE 的封装。
 * @author tomszhou
 * @version 1.1
 * @date 2008-3-19
 */

#ifndef _TTSYS_MESSAGE_QUEUE_H_
#define _TTSYS_MESSAGE_QUEUE_H_

#include "ttdef.h"

__BEGIN_DECLS


/**
 * @defgroup TTSYS_MSGQUEUE 消息组件
 * @brief 系统消息队列组件，对 SYSTEM MESSAGE 的封装。
 * @author tomszhou
 * @version 1.1
 * @date 2008-3-19
 * @ingroup TTSYS 
 * @{
 */


/**
 * TMessageQueue 消息组件抽象定义
 */

typedef struct TMessageQueue  TMessageQueue;
/**
 * TMessageQueue 消息组件结构定义
 */

struct TMessageQueue
{
#define  MESSAGE_QUEUE_NAME_MAX_LEN  32               /**< 消息名称的最大长度  */
  char    name[MESSAGE_QUEUE_NAME_MAX_LEN];           /**< 消息名称     */
  uint32  key;                                        /**< 消息队列键值 */
  int     qid;                                        /**< 消息队列ID   */
  int     flag;                                       /**< 消息创建方式 */
};

/**
 * TMessageQueueBuffer 消息定义
 */

typedef struct TMessageQueueBuffer   TMessageQueueBuffer;

/**
 * TMessageQueueBuffer 消息结构定义
 */
struct TMessageQueueBuffer 
{
  long   mtype;        /**< message type, must be > 0 */
  char   mtext[1];     /**< message data */
};


/** 创建队列
 *  @param queue 创建后的队列对象
 *  @param key  队列ID号
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueCreate(TMessageQueue *queue,uint32 key);

/** 根据名称创建队列
 *  @param queue 创建后的队列对象
 *  @param name  队列名称
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueCreateEx(TMessageQueue *queue,const char *name);

/** 销毁队列
 *  @param key  队列KEY号
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueDestory(uint32 key);

/** 销毁队列
 *  @param qid  队列ID号
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueDestroyByID(uint32 qid);

/** 根据名称销毁队列
 *  @param name  队列名称
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueDestoryEx(const char *name);

/** 打开队列
 *  @param queue 打开后的队列对象
 *  @param key  队列ID号
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueOpen(TMessageQueue *queue,uint32 key);

/** 根据ID打开队列
 *  @param queue 打开后的队列对象
 *  @param qid  队列ID号
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int  MessageQueueOpenByID(TMessageQueue *queue,uint32 qid);

/** 根据名称打开队列
 *  @param queue 打开后的队列对象
 *  @param name  队列名称
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueOpenEx(TMessageQueue *queue,const char *name);

/** 向队列发送数据
 *  @param queue 相对应的队列
 *  @param buffer  数据
 *  @param size 数据大小
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueSend(TMessageQueue *queue,TMessageQueueBuffer *buffer,uint32 size);

/** 从队列接受数据
 *  @param queue 相对应的队列
 *  @param buffer  数据
 *  @param size 数据大小
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueRecv(TMessageQueue *queue,TMessageQueueBuffer *buffer,uint32 size);

/** 从队列接受数据一直到超时时间
 *  @param queue 相对应的队列
 *  @param buffer  数据
 *  @param size 数据大小
 *  @param times 超时时间
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MessageQueueRecvEx(TMessageQueue *queue,TMessageQueueBuffer *buffer,uint32 size,int times);


/** @} */

__END_DECLS

#endif



