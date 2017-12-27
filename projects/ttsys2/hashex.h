
/**
 * @file hashex.h
 * @brief 实现共享HASH表(已修订)
 * @author tomszhou
 * @version 1.0.0
 * @date 2011-4-16
 */

#ifndef _TTSYS_SHMEM_HASHEX_H_
#define _TTSYS_SHMEM_HASHEX_H_

#include "ttdef.h"
#include "shmempool.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_HASHEX 实现共享HASH表。
 * @ingroup TTSYS
 * @{
 */

/**
 *  共享 HASH 通用定义 
 */
enum
{
  HASHEX_KEY_STRING   = -1,   /**< HASH表键值长度标识 */
  HASHEX_INITIAL_MAX  = 127,  /**< HASH最大数127*/
  HASHEX_INITIAL_MAX2 = 255   /**< HASH最大数255*/
};

/**
 *  HASH表索引定义
 */
typedef struct hashex_index_t THasheExIndex;

/**
 *  HASH表定义
 */
typedef struct hashex_t THashEx;

/**
 *  HASH值计算方法定义
 */
typedef unsigned int (*THashExFunc)(const char *key, uint32 *klen);


/** 基于的HASH键值计算方法(33)
 *  @param key   键
 *  @param klen  键长度(可以为HASHEX_KEY_STRING)
 *  @return 返回HASH值
 */
unsigned int HashExFuncDefault33(const char *key, uint32 *klen);

/** 基于的HASH键值计算方法(63)
 *  @param key   键
 *  @param klen  键长度(可以为HASHEX_KEY_STRING)
 *  @return 返回HASH值
 */
unsigned int HashExFuncDefault63(const char *key, uint32 *klen);

/** 创建共享HASHEX
 *  @param pool   内存池
 *  @param spool  共享内存池
 *  @param max    HASH最大数(HASHEX_INITIAL_MAX,HASHEX_INITIAL_MAX2,2^n - 1,...)
 *  @param hash   创建成功后的HASHEX
 *  @param addr   创建成功后的HASHEX的共享地址
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int HashExMake(TMemPool *pool,TShmemPool *spool,int max,THashEx **hash,int *addr);

/** 创建共享HASHEX(63算法)
 *  @param pool   内存池
 *  @param spool  共享内存池
 *  @param max    HASH最大数(质数(457...)
 *  @param hash   创建成功后的HASHEX
 *  @param addr   创建成功后的HASHEX的共享地址
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int HashExMake63(TMemPool *pool,TShmemPool *spool,int max,THashEx **hash,int *addr);


/** 打开共享HASHEX(63算法)
 *  @param pool   内存池
 *  @param spool  共享内存池
 *  @param addr   HASHEX的共享地址
 *  @param hash   打开成功后的HASHEX
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int HashExOpen(TMemPool *pool,TShmemPool *spool,int addr,THashEx **hash);

/** 打开共享HASHEX
 *  @param pool   内存池
 *  @param spool  共享内存池
 *  @param addr   HASHEX的共享地址
 *  @param hash   打开成功后的HASHEX
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int HashExOpen63(TMemPool *pool,TShmemPool *spool,int addr,THashEx **hash);

/** 在共享HASHEX中设置键值
 *  @param ht    共享HASHEX
 *  @param key   键
 *  @param klen  键长度(>0)
 *  @param val   值
 *  @param vlen  值长度(>0)
 *  @return 成功返回TTS_SUCCESS。
 */
int HashExSet(THashEx *ht,const char *key,int klen,const void *val,int vlen);

/** 在共享HASHEX中获取键值
 *  @param ht    共享HASHEX
 *  @param key   键
 *  @param klen  键长度(>0)
 *  @param val   值(输出区)
 *  @param vlen  值长度(可以为NULL)
 *  @return 成功返回TTS_SUCCESS(TTS_NOTFOUND)。
 */
int HashExGet(THashEx *ht,const char *key,int klen,void *val,int *vlen);

/** 在共享HASHEX中获取键值
 *  @param ht    共享HASHEX
 *  @param key   键
 *  @param klen  键长度(>0)
 *  @param val   值(共享区)
 *  @param vlen  值长度(可以为NULL)
 *  @return 成功返回TTS_SUCCESS(TTS_NOTFOUND)。
 */
int HashExGet2(THashEx *ht,const char *key,int klen,void **val,int *vlen);

/** 获取共享HASHEX中元素个数
 *  @param ht    共享HASHEX
 *  @return 返回元素个数。
 */
int HashExCount(THashEx *ht);

/** 根据HASH表索引获取下一个索引
 *  @param hi HASH表索引
 *  @return 成功返回HASH表索引地址； NULL表示已经没有索引 。
 */
THasheExIndex *HashExNext(THasheExIndex *hi);

/** 根据HASH表获取下第一个索引
 *  @param ht HASH表
 *  @return 成功返回HASH表第一个索引地址； NULL表示已经没有索引 。
 */
THasheExIndex *HashExFirst(THashEx *ht);

/** 在共享HASHEX中根据当前索引获取数据
 *  @param hi    HASH表索引
 *  @param key   键(存放)
 *  @param klen  键长度(>0)(存放)
 *  @param val   值(输出区)(存放)
 *  @param vlen  值长度(存放)
 *  @return 成功返回TTS_SUCCESS(TTS_ENULL)。
 */
int HashExThis(THasheExIndex *hi,char **key,int *klen,void **val,int *vlen);

/**
 *  HASH表存储分布状态
 */
typedef struct 
{
  int     bad;         /**< 存储分布 */
  int     total;       /**< 存储个数 */
}THashExStore;

/** 获取HASH表存储分布状态
 *  @param pool   内存池
 *  @param ht     HASH表
 *  @param store  存储分布状态
 *  @param bad    存储分布种类
 *  @return 成功返回TTS_SUCCESS(TTS_ENULL)。
 */
int HashExStorage(TMemPool *pool,THashEx *ht,THashExStore **store,int *bad);


/** @} */

__END_DECLS

#endif



