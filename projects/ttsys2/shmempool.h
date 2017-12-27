
/**
 * @file shmempool.h
 * @brief 单体共享内存池结构定义。(已修订)
 * @author tomszhou
 * @version 1.0.0
 * @date 2011-4-16
 */

#ifndef _TTSYS_SHMEM_POOL_H_
#define _TTSYS_SHMEM_POOL_H_

#include "ttdef.h"
#include "memory.h"
#include "shmem.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_SHMEMPOOL 单体共享内存池数据结构及其相对应函数的定义。(2011/10/7 15:20)
 * @ingroup TTSYS
 * @{
 */

/**
 *  单体共享内存池结构定义
 */
typedef struct stru_shmem_pool_t TShmemPool;

/** 创建单体共享内存池
 *  @param pool    内存池
 *  @param pid     项目ID唯一
 *  @param size    共享内存大小
 *  @param offset  偏移量
 *  @param spool   创建成功后的单体共享内存池
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int ShmemPoolCreate(TMemPool *pool,int pid,int size,int offset,TShmemPool **spool);

/** 校验单体共享内存池是否存在
 *  @param pid   项目ID唯一
 *  @param size  共享内存大小
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int ShmemPoolCheck(int pid,int size);

/** 打开单体共享内存池,并初始化
 *  @param pool  内存池
 *  @param pid   项目ID唯一
 *  @param size  共享内存大小
 *  @param offset 偏移量
 *  @param spool 打开成功后的单体共享内存池
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int ShmemPoolOpenEx(TMemPool *pool,int proj,int size,int offset,TShmemPool **spool);

/** 打开单体共享内存池
 *  @param pool  内存池
 *  @param pid   项目ID唯一
 *  @param size  共享内存大小
 *  @param offset 偏移量
 *  @param spool 打开成功后的单体共享内存池
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int ShmemPoolOpen(TMemPool *pool,int pid,int size,int offset,TShmemPool **spool);

/** 销毁单体共享内存池
 *  @param spool 单体共享内存池
 *  @return 成功返回TTS_SUCCESS。
 */
int ShmemPoolDestory(TShmemPool *spool);

/** 从单体共享内存池中分配空间。
 *  @param spool 单体共享内存池
 *  @param size 所需的空间大小。
 *  @return 成功返回内存地址； 失败返回空 。
 */
char *ShmemPoolAlloc(TShmemPool *spool,int size);

/** 从单体共享内存池中分配空间。
 *  @param spool 单体共享内存池
 *  @param size 所需的空间大小。
 *  @param addr  地址
 *  @return 成功返回内存地址； 失败返回空 。
 */
char *ShmemPoolAllocEx(TShmemPool *spool,int size,int *addr);

/** 根据共享内存首存储信息地址
 *  @param spool 单体共享内存池
 *  @param size 大小
 *  @return 成功返回内存地址； 失败返回空 。
 */
char *ShmemPoolGetBaseAddr(TShmemPool *spool,int size);

/** 根据偏移量获取共享内存的地址
 *  @param spool 单体共享内存池
 *  @param offset 偏移量
 *  @return 成功返回内存地址； 失败返回空 。
 */
char *ShmemPoolGetAddr(TShmemPool *spool,int offset);

/** 根据共享内存的信息
 *  @param spool 单体共享内存池
 *  @param shmem 共享内存信息
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int ShmemPoolGetShmem(TShmemPool *spool, TShmem *shmem);

/** 获取共享内存地址的偏移量
 *  @param spool 单体共享内存池
 *  @param data 数据地址
 *  @return 成功返回内存地址； 失败返回空 。
 */
int ShmemGetOffset(TShmemPool *spool,char *data);

/** 获取共享内存池已使用大小
 *  @param spool 单体共享内存池
 *  @return 成功返回内存地址； 失败返回空 。
 */
uint32 ShmemPoolGetUsed(TShmemPool *spool);

/** 获取共享内存池空闲大小
 *  @param spool 单体共享内存池
 *  @return 成功返回内存地址； 失败返回空 。
 */
uint32 ShmemPoolGetFree(TShmemPool *spool);

/** 获取共享内存池大小
 *  @param spool 单体共享内存池
 *  @return 成功返回内存地址； 失败返回空 。
 */
uint32 ShmemPoolGetTotal(TShmemPool *spool);

/** @} */

#ifdef __cplusplus
}
#endif

#endif



