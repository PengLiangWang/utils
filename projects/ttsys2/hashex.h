
/**
 * @file hashex.h
 * @brief ʵ�ֹ���HASH��(���޶�)
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
 * @defgroup TTSYS_HASHEX ʵ�ֹ���HASH��
 * @ingroup TTSYS
 * @{
 */

/**
 *  ���� HASH ͨ�ö��� 
 */
enum
{
  HASHEX_KEY_STRING   = -1,   /**< HASH���ֵ���ȱ�ʶ */
  HASHEX_INITIAL_MAX  = 127,  /**< HASH�����127*/
  HASHEX_INITIAL_MAX2 = 255   /**< HASH�����255*/
};

/**
 *  HASH����������
 */
typedef struct hashex_index_t THasheExIndex;

/**
 *  HASH����
 */
typedef struct hashex_t THashEx;

/**
 *  HASHֵ���㷽������
 */
typedef unsigned int (*THashExFunc)(const char *key, uint32 *klen);


/** ���ڵ�HASH��ֵ���㷽��(33)
 *  @param key   ��
 *  @param klen  ������(����ΪHASHEX_KEY_STRING)
 *  @return ����HASHֵ
 */
unsigned int HashExFuncDefault33(const char *key, uint32 *klen);

/** ���ڵ�HASH��ֵ���㷽��(63)
 *  @param key   ��
 *  @param klen  ������(����ΪHASHEX_KEY_STRING)
 *  @return ����HASHֵ
 */
unsigned int HashExFuncDefault63(const char *key, uint32 *klen);

/** ��������HASHEX
 *  @param pool   �ڴ��
 *  @param spool  �����ڴ��
 *  @param max    HASH�����(HASHEX_INITIAL_MAX,HASHEX_INITIAL_MAX2,2^n - 1,...)
 *  @param hash   �����ɹ����HASHEX
 *  @param addr   �����ɹ����HASHEX�Ĺ����ַ
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int HashExMake(TMemPool *pool,TShmemPool *spool,int max,THashEx **hash,int *addr);

/** ��������HASHEX(63�㷨)
 *  @param pool   �ڴ��
 *  @param spool  �����ڴ��
 *  @param max    HASH�����(����(457...)
 *  @param hash   �����ɹ����HASHEX
 *  @param addr   �����ɹ����HASHEX�Ĺ����ַ
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int HashExMake63(TMemPool *pool,TShmemPool *spool,int max,THashEx **hash,int *addr);


/** �򿪹���HASHEX(63�㷨)
 *  @param pool   �ڴ��
 *  @param spool  �����ڴ��
 *  @param addr   HASHEX�Ĺ����ַ
 *  @param hash   �򿪳ɹ����HASHEX
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int HashExOpen(TMemPool *pool,TShmemPool *spool,int addr,THashEx **hash);

/** �򿪹���HASHEX
 *  @param pool   �ڴ��
 *  @param spool  �����ڴ��
 *  @param addr   HASHEX�Ĺ����ַ
 *  @param hash   �򿪳ɹ����HASHEX
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int HashExOpen63(TMemPool *pool,TShmemPool *spool,int addr,THashEx **hash);

/** �ڹ���HASHEX�����ü�ֵ
 *  @param ht    ����HASHEX
 *  @param key   ��
 *  @param klen  ������(>0)
 *  @param val   ֵ
 *  @param vlen  ֵ����(>0)
 *  @return �ɹ�����TTS_SUCCESS��
 */
int HashExSet(THashEx *ht,const char *key,int klen,const void *val,int vlen);

/** �ڹ���HASHEX�л�ȡ��ֵ
 *  @param ht    ����HASHEX
 *  @param key   ��
 *  @param klen  ������(>0)
 *  @param val   ֵ(�����)
 *  @param vlen  ֵ����(����ΪNULL)
 *  @return �ɹ�����TTS_SUCCESS(TTS_NOTFOUND)��
 */
int HashExGet(THashEx *ht,const char *key,int klen,void *val,int *vlen);

/** �ڹ���HASHEX�л�ȡ��ֵ
 *  @param ht    ����HASHEX
 *  @param key   ��
 *  @param klen  ������(>0)
 *  @param val   ֵ(������)
 *  @param vlen  ֵ����(����ΪNULL)
 *  @return �ɹ�����TTS_SUCCESS(TTS_NOTFOUND)��
 */
int HashExGet2(THashEx *ht,const char *key,int klen,void **val,int *vlen);

/** ��ȡ����HASHEX��Ԫ�ظ���
 *  @param ht    ����HASHEX
 *  @return ����Ԫ�ظ�����
 */
int HashExCount(THashEx *ht);

/** ����HASH��������ȡ��һ������
 *  @param hi HASH������
 *  @return �ɹ�����HASH��������ַ�� NULL��ʾ�Ѿ�û������ ��
 */
THasheExIndex *HashExNext(THasheExIndex *hi);

/** ����HASH���ȡ�µ�һ������
 *  @param ht HASH��
 *  @return �ɹ�����HASH���һ��������ַ�� NULL��ʾ�Ѿ�û������ ��
 */
THasheExIndex *HashExFirst(THashEx *ht);

/** �ڹ���HASHEX�и��ݵ�ǰ������ȡ����
 *  @param hi    HASH������
 *  @param key   ��(���)
 *  @param klen  ������(>0)(���)
 *  @param val   ֵ(�����)(���)
 *  @param vlen  ֵ����(���)
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENULL)��
 */
int HashExThis(THasheExIndex *hi,char **key,int *klen,void **val,int *vlen);

/**
 *  HASH��洢�ֲ�״̬
 */
typedef struct 
{
  int     bad;         /**< �洢�ֲ� */
  int     total;       /**< �洢���� */
}THashExStore;

/** ��ȡHASH��洢�ֲ�״̬
 *  @param pool   �ڴ��
 *  @param ht     HASH��
 *  @param store  �洢�ֲ�״̬
 *  @param bad    �洢�ֲ�����
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENULL)��
 */
int HashExStorage(TMemPool *pool,THashEx *ht,THashExStore **store,int *bad);


/** @} */

__END_DECLS

#endif



