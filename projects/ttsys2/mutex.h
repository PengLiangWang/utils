
/**
 * @file mutex.h
 * @brief 系统信号量组件，对 SYSTEM MUTEX 的封装。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-21
 */


#ifndef _TTSYS_MUTEX_H_
#define _TTSYS_MUTEX_H_

#include "ttdef.h"



__BEGIN_DECLS

/**
 * @defgroup TTSYS_MUTEX 信号量组件
 * @brief 系统信号量组件，对 SYSTEM MUTEX 的封装。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-21
 * @ingroup TTSYS
 * @{
 */

/**
 * TMutex 信号量组件抽象定义
 */

typedef struct TMutex  TMutex;  

/**
 * TMutex 信号量组件定义
 */

struct TMutex
{
  uint32     semkey;                      /**< 信号量KEY值 */
  int        semid;                       /**< 信号量ID号 */
  int        nsems;                       /**< 信号量个数*/
  int        curr_locked;                 /**< 当前锁 */
#define  MUTEX_NAME_MAX_LEN  32           /**< 信号量名称的最大长度 */
  char       name[MUTEX_NAME_MAX_LEN+1];  /**< 信号量名称 */
};

/** 创建信号量集对象
 *  @param mutex 存放对象的指针
 *  @param key 信号量ID号
 *  @param nsems 信号数量
 *  @param name 信号量名称
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MutexCreate(TMutex *mutex,int key,int nsems,const char *name);

/** 销毁信号量集对象
 *  @param mutex 信号量集对象
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MutexDestory(TMutex *mutex);

/** 打开信号量集对象
 *  @param mutex 存放对象的指针
 *  @param key 信号量ID号
 *  @param nsems 信号数量
 *  @param name 信号量名称
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MutexOpen(TMutex *mutex,int key,int nsems,const char *name);

/** 上锁
 *  @param mutex 信号量集
 *  @param nsem  信号量
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MutexAcquire(TMutex *mutex,int nsem);

/** 解锁
 *  @param mutex 信号量集
 *  @param nsem  信号量
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int MutexRelease(TMutex *mutex,int nsem);


/** @} */

__END_DECLS


#endif




