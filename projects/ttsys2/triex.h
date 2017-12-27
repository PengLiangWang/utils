
/**
 * @file triex.h
 * @brief 共享TRIE树
 * @author tomszhou
 * @version 1.0.0
 * @date 2011-4-16
 */

#ifndef _TTSYS_SHMEM_TRIEEX_H_
#define _TTSYS_SHMEM_TRIEEX_H_

#include "ttdef.h"
#include "shmempool.h"

__BEGIN_DECLS
/**
 * @defgroup TTSYS_TRIEX 实现共享TRIE树。
 * @ingroup TTSYS
 * @{
 */

/**
 *  TRIEX树定义
 */
typedef struct stru_trie_ex  TTrieEx;

/** 创建共享TRIE
 *  @param pool   内存池
 *  @param spool  共享内存池
 *  @param trieex 创建成功后的TRIE
 *  @param addr   创建成功后的TRIE的共享地址
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int TrieExMake(TMemPool *pool,TShmemPool *spool,TTrieEx **trieex,int *addr);

/** 打开共享TRIE
 *  @param pool   内存池
 *  @param spool  共享内存池
 *  @param addr   TRIE的共享地址
 *  @param trieex 打开成功后的TRIE
 *  @return 成功返回TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)。
 */
int TrieExOpen(TMemPool *pool,TShmemPool *spool,int addr,TTrieEx **trieex);

/** 在共享TRIE中设置键值
 *  @param trie  共享TRIE
 *  @param name  键
 *  @param val   值
 *  @param vlen  值长度(>0)
 *  @return 成功返回TTS_SUCCESS。
 */
int TrieExSet(TTrieEx *trie,char  *name,void *val,int vlen);

/** 在共享TRIE中获取键值
 *  @param trie  共享TRIE
 *  @param name  键
 *  @param val   值(输出区)
 *  @param vlen  值长度(可以为NULL)
 *  @return 成功返回TTS_SUCCESS(TTS_NOTFOUND)。
 */
int TrieExGet(TTrieEx *trie,char  *name,void *val,int *vlen);

/** 打印TRIE中所有节点
 *  @param trie  共享TRIE
 *  @param maxlen  键的最大长度
 *  @return 成功返回TTS_SUCCESS
 */
int TrieExPrint(TTrieEx *trie,int maxlen);

/** @} */

__END_DECLS

#endif
