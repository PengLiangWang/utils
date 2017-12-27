
/**
 * @file memory.h
 * @brief �ڴ�����ݽṹ�������Ӧ�����Ķ��塣(���޶�)
 * @author tomszhou
 * @version 1.1.1
 * @date 2008-3-19
 */


#ifndef _TTSYS_MEMORY_POOL_H_
#define _TTSYS_MEMORY_POOL_H_

#include "ttdef.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_MEMORY �ڴ�����ݽṹ�������Ӧ�����Ķ��塣
 * @ingroup TTSYS
 * @{
 */

/**
 * �ڴ��(TMemPool)�ṹ���塣
 */
typedef struct TMemPool    TMemPool;

/** �����ڴ�ء�
 *  @param newpool ��������ڴ�ء�
 *  @param parent ���ڴ��
 *��@param size   �ڴ�صĴ�С
 *  @return �ɹ�����TTS_SUCCESS�� ʧ�ܷ���TTS_ENOMEM ��
 */
int  MemPoolCreate(TMemPool **newpool,TMemPool *parent,uint32 size);

/** �����ڴ�ء�
 *  @param pool ��������ڴ�ء�
 *  @param parent ��һ�ڴ��
 *  @return �ɹ�����TTS_SUCCESS�� ʧ�ܷ���TTS_ENOMEM ��
 */
int MemPoolCreateEx(TMemPool **pool,TMemPool *parent);

/** ���ڴ���з���ռ䡣
 *  @param pool ���ڴ��
 *  @param size ����Ŀռ��С��
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
void *MemPoolAlloc(TMemPool *pool,uint32 size);

/** ���ڴ���з���ռ�,�����ڴ�ռ���Ϊ 0 ��
 *  @param pool ������ڴ�ء�
 *  @param size ����Ŀռ��С��
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
void *MemPoolAllocEx(TMemPool *pool,uint32 size);

/** ����ڴ�ء�
 *  @param pool ��Ҫ������ڴ�ء�
 *  @return 
 */
void MemPoolClearEx(TMemPool *pool);

/** �����ڴ�ء�
 *  @param pool ��Ҫ���ٵ��ڴ�ء�
 *  @return 
 */
void MemPoolDestoryEx(TMemPool *pool);

/** �Ǽ��ڴ�����������ʱ�����ٵ���Դ��
 *  @param pool ����Ӧ���ڴ�ء�
 *  @param data ��Դ����
 *  @param cleanup_fn ������Դ���Ӧ�ĺ���
 *  @return 
 */
void MemPoolRegisterCleanup(TMemPool * pool, void *data,int (*cleanup_fn)(void *data));

/** ȡ���ڴ�����������ʱ�����ٵ���Դ��
 *  @param pool ����Ӧ���ڴ�ء�
 *  @param data ��Դ����
 *  @param cleanup_fn ������Դ���Ӧ�ĺ���
 *  @return 
 */
void MemPoolUnRegisterCleanup(TMemPool * pool, void *data,int (*cleanup_fn)(void *));


/** ����ڴ�����������ʱ�����ٵ���Դ�������ڴ����ɾ���ö�����
 *  @param pool ����Ӧ���ڴ�ء�
 *  @param data ��Դ����
 *  @param cleanup_fn ������Դ���Ӧ�ĺ���
 *  @return 
 */
int MemPoolRunCleanup(TMemPool * pool, void *data,int (*cleanup_fn)(void *));

/** @} */

__END_DECLS

#endif


