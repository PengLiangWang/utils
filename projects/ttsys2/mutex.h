
/**
 * @file mutex.h
 * @brief ϵͳ�ź���������� SYSTEM MUTEX �ķ�װ��
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-21
 */


#ifndef _TTSYS_MUTEX_H_
#define _TTSYS_MUTEX_H_

#include "ttdef.h"



__BEGIN_DECLS

/**
 * @defgroup TTSYS_MUTEX �ź������
 * @brief ϵͳ�ź���������� SYSTEM MUTEX �ķ�װ��
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-21
 * @ingroup TTSYS
 * @{
 */

/**
 * TMutex �ź������������
 */

typedef struct TMutex  TMutex;  

/**
 * TMutex �ź����������
 */

struct TMutex
{
  uint32     semkey;                      /**< �ź���KEYֵ */
  int        semid;                       /**< �ź���ID�� */
  int        nsems;                       /**< �ź�������*/
  int        curr_locked;                 /**< ��ǰ�� */
#define  MUTEX_NAME_MAX_LEN  32           /**< �ź������Ƶ���󳤶� */
  char       name[MUTEX_NAME_MAX_LEN+1];  /**< �ź������� */
};

/** �����ź���������
 *  @param mutex ��Ŷ����ָ��
 *  @param key �ź���ID��
 *  @param nsems �ź�����
 *  @param name �ź�������
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int MutexCreate(TMutex *mutex,int key,int nsems,const char *name);

/** �����ź���������
 *  @param mutex �ź���������
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int MutexDestory(TMutex *mutex);

/** ���ź���������
 *  @param mutex ��Ŷ����ָ��
 *  @param key �ź���ID��
 *  @param nsems �ź�����
 *  @param name �ź�������
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int MutexOpen(TMutex *mutex,int key,int nsems,const char *name);

/** ����
 *  @param mutex �ź�����
 *  @param nsem  �ź���
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int MutexAcquire(TMutex *mutex,int nsem);

/** ����
 *  @param mutex �ź�����
 *  @param nsem  �ź���
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int MutexRelease(TMutex *mutex,int nsem);


/** @} */

__END_DECLS


#endif




