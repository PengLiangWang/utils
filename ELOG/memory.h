
/**
 * @file memory.h
 * @brief 内存池数据结构及其相对应函数的定义。(已修订)
 * @author tomszhou
 * @version 1.1.1
 * @date 2008-3-19
 */


#ifndef _TTSYS_MEMORY_POOL_H_
#define _TTSYS_MEMORY_POOL_H_

#include "ttdef.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_MEMORY 内存池数据结构及其相对应函数的定义。
 * @ingroup TTSYS
 * @{
 */

/**
 * 内存池(TMemPool)结构定义。
 */
typedef struct TMemPool    TMemPool;

/** 创建内存池。
 *  @param newpool 创建后的内存池。
 *  @param parent 父内存池
 *　@param size   内存池的大小
 *  @return 成功返回TTS_SUCCESS； 失败返回TTS_ENOMEM 。
 */
int  MemPoolCreate(TMemPool **newpool,TMemPool *parent,uint32 size);

/** 创建内存池。
 *  @param pool 创建后的内存池。
 *  @param parent 上一内存池
 *  @return 成功返回TTS_SUCCESS； 失败返回TTS_ENOMEM 。
 */
int MemPoolCreateEx(TMemPool **pool,TMemPool *parent);

/** 从内存池中分配空间。
 *  @param pool 根内存池
 *  @param size 所需的空间大小。
 *  @return 成功返回内存地址； 失败返回空 。
 */
void *MemPoolAlloc(TMemPool *pool,uint32 size);

/** 从内存池中分配空间,并将内存空间置为 0 。
 *  @param pool 所需的内存池。
 *  @param size 所需的空间大小。
 *  @return 成功返回内存地址； 失败返回空 。
 */
void *MemPoolAllocEx(TMemPool *pool,uint32 size);

/** 清空内存池。
 *  @param pool 所要清除的内存池。
 *  @return 
 */
void MemPoolClearEx(TMemPool *pool);

/** 销毁内存池。
 *  @param pool 所要销毁的内存池。
 *  @return 
 */
void MemPoolDestoryEx(TMemPool *pool);

/** 登记内存池清除或销毁时需销毁的资源。
 *  @param pool 所对应的内存池。
 *  @param data 资源数据
 *  @param cleanup_fn 销毁资源相对应的函数
 *  @return 
 */
void MemPoolRegisterCleanup(TMemPool * pool, void *data,int (*cleanup_fn)(void *data));

/** 取消内存池清除或销毁时需销毁的资源。
 *  @param pool 所对应的内存池。
 *  @param data 资源数据
 *  @param cleanup_fn 销毁资源相对应的函数
 *  @return 
 */
void MemPoolUnRegisterCleanup(TMemPool * pool, void *data,int (*cleanup_fn)(void *));


/** 清除内存池清除或销毁时需销毁的资源，并从内存池中删除该动作。
 *  @param pool 所对应的内存池。
 *  @param data 资源数据
 *  @param cleanup_fn 销毁资源相对应的函数
 *  @return 
 */
int MemPoolRunCleanup(TMemPool * pool, void *data,int (*cleanup_fn)(void *));

/** @} */

__END_DECLS

#endif


