
/**
 * @file triex.h
 * @brief ����TRIE��
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
 * @defgroup TTSYS_TRIEX ʵ�ֹ���TRIE����
 * @ingroup TTSYS
 * @{
 */

/**
 *  TRIEX������
 */
typedef struct stru_trie_ex  TTrieEx;

/** ��������TRIE
 *  @param pool   �ڴ��
 *  @param spool  �����ڴ��
 *  @param trieex �����ɹ����TRIE
 *  @param addr   �����ɹ����TRIE�Ĺ����ַ
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int TrieExMake(TMemPool *pool,TShmemPool *spool,TTrieEx **trieex,int *addr);

/** �򿪹���TRIE
 *  @param pool   �ڴ��
 *  @param spool  �����ڴ��
 *  @param addr   TRIE�Ĺ����ַ
 *  @param trieex �򿪳ɹ����TRIE
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int TrieExOpen(TMemPool *pool,TShmemPool *spool,int addr,TTrieEx **trieex);

/** �ڹ���TRIE�����ü�ֵ
 *  @param trie  ����TRIE
 *  @param name  ��
 *  @param val   ֵ
 *  @param vlen  ֵ����(>0)
 *  @return �ɹ�����TTS_SUCCESS��
 */
int TrieExSet(TTrieEx *trie,char  *name,void *val,int vlen);

/** �ڹ���TRIE�л�ȡ��ֵ
 *  @param trie  ����TRIE
 *  @param name  ��
 *  @param val   ֵ(�����)
 *  @param vlen  ֵ����(����ΪNULL)
 *  @return �ɹ�����TTS_SUCCESS(TTS_NOTFOUND)��
 */
int TrieExGet(TTrieEx *trie,char  *name,void *val,int *vlen);

/** ��ӡTRIE�����нڵ�
 *  @param trie  ����TRIE
 *  @param maxlen  ������󳤶�
 *  @return �ɹ�����TTS_SUCCESS
 */
int TrieExPrint(TTrieEx *trie,int maxlen);

/** @} */

__END_DECLS

#endif
