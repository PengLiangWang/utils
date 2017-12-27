
/**
 * @file shmempool.h
 * @brief ���干���ڴ�ؽṹ���塣(���޶�)
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
 * @defgroup TTSYS_SHMEMPOOL ���干���ڴ�����ݽṹ�������Ӧ�����Ķ��塣(2011/10/7 15:20)
 * @ingroup TTSYS
 * @{
 */

/**
 *  ���干���ڴ�ؽṹ����
 */
typedef struct stru_shmem_pool_t TShmemPool;

/** �������干���ڴ��
 *  @param pool    �ڴ��
 *  @param pid     ��ĿIDΨһ
 *  @param size    �����ڴ��С
 *  @param offset  ƫ����
 *  @param spool   �����ɹ���ĵ��干���ڴ��
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int ShmemPoolCreate(TMemPool *pool,int pid,int size,int offset,TShmemPool **spool);

/** У�鵥�干���ڴ���Ƿ����
 *  @param pid   ��ĿIDΨһ
 *  @param size  �����ڴ��С
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int ShmemPoolCheck(int pid,int size);

/** �򿪵��干���ڴ��,����ʼ��
 *  @param pool  �ڴ��
 *  @param pid   ��ĿIDΨһ
 *  @param size  �����ڴ��С
 *  @param offset ƫ����
 *  @param spool �򿪳ɹ���ĵ��干���ڴ��
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int ShmemPoolOpenEx(TMemPool *pool,int proj,int size,int offset,TShmemPool **spool);

/** �򿪵��干���ڴ��
 *  @param pool  �ڴ��
 *  @param pid   ��ĿIDΨһ
 *  @param size  �����ڴ��С
 *  @param offset ƫ����
 *  @param spool �򿪳ɹ���ĵ��干���ڴ��
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int ShmemPoolOpen(TMemPool *pool,int pid,int size,int offset,TShmemPool **spool);

/** ���ٵ��干���ڴ��
 *  @param spool ���干���ڴ��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int ShmemPoolDestory(TShmemPool *spool);

/** �ӵ��干���ڴ���з���ռ䡣
 *  @param spool ���干���ڴ��
 *  @param size ����Ŀռ��С��
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
char *ShmemPoolAlloc(TShmemPool *spool,int size);

/** �ӵ��干���ڴ���з���ռ䡣
 *  @param spool ���干���ڴ��
 *  @param size ����Ŀռ��С��
 *  @param addr  ��ַ
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
char *ShmemPoolAllocEx(TShmemPool *spool,int size,int *addr);

/** ���ݹ����ڴ��״洢��Ϣ��ַ
 *  @param spool ���干���ڴ��
 *  @param size ��С
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
char *ShmemPoolGetBaseAddr(TShmemPool *spool,int size);

/** ����ƫ������ȡ�����ڴ�ĵ�ַ
 *  @param spool ���干���ڴ��
 *  @param offset ƫ����
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
char *ShmemPoolGetAddr(TShmemPool *spool,int offset);

/** ���ݹ����ڴ����Ϣ
 *  @param spool ���干���ڴ��
 *  @param shmem �����ڴ���Ϣ
 *  @return �ɹ�����TTS_SUCCESS(TTS_ENOMEM,TTS_EACCES,errno)��
 */
int ShmemPoolGetShmem(TShmemPool *spool, TShmem *shmem);

/** ��ȡ�����ڴ��ַ��ƫ����
 *  @param spool ���干���ڴ��
 *  @param data ���ݵ�ַ
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
int ShmemGetOffset(TShmemPool *spool,char *data);

/** ��ȡ�����ڴ����ʹ�ô�С
 *  @param spool ���干���ڴ��
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
uint32 ShmemPoolGetUsed(TShmemPool *spool);

/** ��ȡ�����ڴ�ؿ��д�С
 *  @param spool ���干���ڴ��
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
uint32 ShmemPoolGetFree(TShmemPool *spool);

/** ��ȡ�����ڴ�ش�С
 *  @param spool ���干���ڴ��
 *  @return �ɹ������ڴ��ַ�� ʧ�ܷ��ؿ� ��
 */
uint32 ShmemPoolGetTotal(TShmemPool *spool);

/** @} */

#ifdef __cplusplus
}
#endif

#endif



