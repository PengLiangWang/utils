
/**
 * @file shmem.h
 * @brief ϵͳ�����ڴ�������� SYSTEM SHARE MEMORY �ķ�װ��
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-20
 */


#ifndef _TTSYS_SHMEM_H_
#define _TTSYS_SHMEM_H_

#include "ttdef.h"

__BEGIN_DECLS


/**
 * @defgroup TTSYS_SHMEM �����ڴ����
 * @brief ϵͳ�����ڴ�������� SYSTEM SHARE MEMORY �ķ�װ��
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-20
 * @ingroup TTSYS 
 * @{
 */


/**
 * TShmem �����ڴ����������
 */
typedef  struct TShmem TShmem;

/**
 * TShmem �����ڴ��������
 */

struct TShmem
{
  uint32      key;             /**< �����ڴ�KEYֵ */
  void       *base;            /**< �����ڴ���ʼ��ַ */
  void       *usable;          /**< �����ڴ���ʼ��ַ */
  uint32      size;            /**< �����ڴ��С */
  int         shmid;           /**< �����ڴ�ID�� */
  int         nattch;          /**< �û������� */
};

/** ���������ڴ�
 *  @param shmem ��Ŷ����ָ��
 *  @param size  ��С
 *  @param key  �����ڴ�ID
 *  @param shmbase �����ڴ����ַ
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int ShmemCreate(TShmem *shmem,uint32 size,int key,ulong32 shmbase);

/** ���ӹ����ڴ�
 *  @param shmem ��Ŷ����ָ��
 *  @param size  ��С
 *  @param key  �����ڴ�ID
 *  @param shmbase �����ڴ����ַ
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int ShmemAttach(TShmem *shmem,uint32 size,int key,ulong32 shmbase);

/** �Ͽ��빲���ڴ������
 *  @param shmem ��Ŷ����ָ��
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int ShmemDetach(TShmem *shmem);

/** ���ٹ����ڴ�
 *  @param shmem �����ڴ����
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int ShmemDestory(TShmem *shmem);

/** ��ȡ�����ڴ浱ǰ������
 *  @param shmem �����ڴ����
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int ShmemGetNattch(TShmem *shmem);

/** ��ȡ�����ڴ�Ļ���ַ
 *  @param shmem �����ڴ����
 *  @return �ɹ����ص�ַ��ʧ�ܷ���NULL��
 */
void *ShmemGetBaseAddress(const TShmem *shmem);

/** ��ȡ�����ڴ�Ĵ�С
 *  @param shmem �����ڴ����
 *  @return ���ع����ڴ�Ĵ�С
 */
uint32 ShmemGetSize(const TShmem *shmem);

/** @} */

__END_DECLS


#endif
