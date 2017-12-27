
/**
 * @file shmem.h
 * @brief 系统共享内存组件，对 SYSTEM SHARE MEMORY 的封装。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-20
 */


#ifndef _TTSYS_SHMEM_H_
#define _TTSYS_SHMEM_H_

#include "ttdef.h"

__BEGIN_DECLS


/**
 * @defgroup TTSYS_SHMEM 共享内存组件
 * @brief 系统共享内存组件，对 SYSTEM SHARE MEMORY 的封装。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-20
 * @ingroup TTSYS 
 * @{
 */


/**
 * TShmem 共享内存组件抽象定义
 */
typedef  struct TShmem TShmem;

/**
 * TShmem 共享内存组件定义
 */

struct TShmem
{
  uint32      key;             /**< 共享内存KEY值 */
  void       *base;            /**< 共享内存起始地址 */
  void       *usable;          /**< 共享内存起始地址 */
  uint32      size;            /**< 共享内存大小 */
  int         shmid;           /**< 共享内存ID号 */
  int         nattch;          /**< 用户连接数 */
};

/** 创建共享内存
 *  @param shmem 存放对象的指针
 *  @param size  大小
 *  @param key  共享内存ID
 *  @param shmbase 共享内存基地址
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int ShmemCreate(TShmem *shmem,uint32 size,int key,ulong32 shmbase);

/** 链接共享内存
 *  @param shmem 存放对象的指针
 *  @param size  大小
 *  @param key  共享内存ID
 *  @param shmbase 共享内存基地址
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int ShmemAttach(TShmem *shmem,uint32 size,int key,ulong32 shmbase);

/** 断开与共享内存的链接
 *  @param shmem 存放对象的指针
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int ShmemDetach(TShmem *shmem);

/** 销毁共享内存
 *  @param shmem 共享内存对象
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int ShmemDestory(TShmem *shmem);

/** 获取共享内存当前连接数
 *  @param shmem 共享内存对象
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int ShmemGetNattch(TShmem *shmem);

/** 获取共享内存的基地址
 *  @param shmem 共享内存对象
 *  @return 成功返回地址；失败返回NULL。
 */
void *ShmemGetBaseAddress(const TShmem *shmem);

/** 获取共享内存的大小
 *  @param shmem 共享内存对象
 *  @return 返回共享内存的大小
 */
uint32 ShmemGetSize(const TShmem *shmem);

/** @} */

__END_DECLS


#endif
