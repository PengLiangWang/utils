
/**
 * @file config.h
 * @brief  �����ļ��������
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-9
 */


#ifndef _TTSYS_CONFIG_H_
#define _TTSYS_CONFIG_H_

#include "ttdef.h"
#include "memory.h"
#include "hlist.h"

__BEGIN_DECLS



/**
 * @defgroup TTSYS_CONFIG �����ļ��������
 * @ingroup TTSYS
 * @{
 */

/**
 * �����ļ����ݴ洢�ṹ����
 */
typedef struct TConfig TConfig;

/**
 * �������
 */
typedef struct TProperty
{
  char   *key;    /**< �� */
  char   *value;  /**< ֵ */
}TProperty;


/**
 *  �����궨��
 */
#define   ConfigFreeData      ConfigFree

/** װ�������ļ�
 *  @param filename �����ļ�����
 *  @param config  �����ļ����ݴ洢��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int  ConfigLoad(const char *filename,TConfig **config);

/** �ر������ļ�
 *  @param config  �����ļ����ݴ洢��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int  ConfigClose(TConfig **config);

/** �������ļ��л�ȡ����ֵ
 *  @param config �����ļ����ݴ洢��
 *  @param section  ������
 *  @param key ��
 *  @param data ֵ
 *  @return �ɹ�����TTS_SUCCESS������TTS_ENULL��ʾû�д˼���
 */
int  ConfigGetProperty(TConfig *config,char *section,char *key,char **data);

/** �ͷŴ洢��������
 *  @param data ֵ
 *  @return �ɹ�����TTS_SUCCESS������TTS_ENULL��ʾû�д˼���
 */
int  ConfigFree(char **data);

/** �������ļ��л�ȡSection�б�
 *  @param config �����ļ����ݴ洢��
 *  @param sections  ��Section�б�
 *  @param nums     Seciont�ĸ���
 *  @return �ɹ�����TTS_SUCCESS������TTS_NOTFOUND��ʾû�д˼���
 */

int  ConfigGetSections(TConfig *config,char ***sections,int *nums);

/** �ͷŴ洢��������
 *  @param sections ֵ
 *  @return �ɹ�����TTS_SUCCESS������TTS_ENULL��ʾû�д˼���
 */
int  ConfigFreeSections(char ***sections);

/** �������ļ��и���Section��ȡKEY�б�
 *  @param config �����ļ����ݴ洢��
 *  @param section section����
 *  @param pros  Property
 *  @param nums  Property�ĸ���
 *  @return �ɹ�����TTS_SUCCESS������TTS_ENULL��ʾû�д˼���
 */
int  ConfigGetPropertys(TConfig *config,char *section,TProperty **pros,int *nums);

/** �ͷŴ洢��������
 *  @param pros ֵ
 *  @return �ɹ�����TTS_SUCCESS������TTS_ENULL��ʾû�д˼���
 */
int  ConfigFreePropertys(TProperty **pros);


/** �������ļ��л�ȡ��������
 *  @param config �����ļ����ݴ洢��
 *  @param section  ������
 *  @param key ��
 *  @param subkey �Ӽ�
 *  @param data ���ݴ洢��
 *  @return �ɹ�����TTS_SUCCESS������TTS_NOTFOUND��ʾû�д˼���
 */
int  ConfigGetSubProperty(TConfig *config,char *section,char *key,char *subkey,char **data);

/** @} */

__END_DECLS

#endif
