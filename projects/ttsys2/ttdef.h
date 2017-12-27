
/**
 * @file ttdef.h
 * @brief TUOTUO ��������Ͷ���
 * @author tuotuo
 * @version 1.0
 * @date 2008-3-1
 */

#ifndef _TTSYS_DEF_H_
#define _TTSYS_DEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/**
 * @defgroup TTSYS_DEF �������ͼ���������
 * @brief �������ͼ���������
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-1
 * @ingroup TTSYS
 * @{
 */

/**
 * @name ���Ͷ���
 * @{
 */


/**
 *  C++�궨��
 */
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#else
#define __BEGIN_DECLS
#endif

/**
 *  C++�궨��
 */
#ifdef __cplusplus
#define ___END_DECLS  }
#else
#define __END_DECLS
#endif

#include <errno.h>

__BEGIN_DECLS

/**
  * �޷������ζ���
  */
typedef unsigned int    uint32;

/**
  * �޷����ַ�����
  */
typedef unsigned char   uchar;

/**
  * �޷��ų����Ͷ���
  */
typedef unsigned long   ulong32;


/** @} */

/**
 * @name ������붨��
 * @{
 */

enum  TTSYS_ERRNO
{
   TTS_MAX_SYS_ERRNO       = 130,       /**< ϵͳ������NO */
   TTS_SUCCESS             = 0,         /**< �ɹ�SUCCESS*/
   TTS_OS_START_ERROR      = 60000,     /**< errno define start */
   TTS_ENOMEM              = 60001,     /**< �ڴ�ռ䲻�� */
   TTS_EGENERAL            = 60002,     /**< ���ڹ��ϣ�������Ϣ���� */
   TTS_EINVAL              = 60003,     /**< ��Чֵ*/
   TTS_EACCES              = 60004,     /**< û��Ȩ��*/
   TTS_TIMEOUT             = 60005,     /**< ������ʱ*/
   TTS_NOTFOUND            = 60006,     /**< "δ�ҵ�Ŀ�����" */
   TTS_EXIST               = 60007,     /**< �����Ѿ����� */
   TTS_ENONE               = 60008,     /**< ���󲻴��� */
   TTS_DLOPEN              = 60009,     /**< �򲻿���̬��(dlopen) */
   TTS_ENULL               = 60010,     /**< ����Ϊ�� */
   TTS_EOF                 = 60011,     /**< �ļ����� */
   TTS_DATEBASE            = 60012,     /**< ���ݿ�ʧ�� */
   TTS_USER_START_ERROR    = 65000      /**< �û��Զ������ */
};

/** @} */


/**
 * @name �������Ͷ���
 * @{
 */

enum  TTSYS_STRING_TYPE
{
   STRING_TYPE_ASC         = 'A',       /**< ASCII�� */
   STRING_TYPE_LBCD        = 'L',       /**< ��BCD�� */
   STRING_TYPE_RBCD        = 'R',       /**< ��BCD�� */
   STRING_TYPE_HEX         = 'H',        /**< HEX��   */
   STRING_TYPE_ETLBCD      = 'X'        /**< HEX��   */
};

/** @} */

/**
  * �������� ��ʽ������ṹ
  */
typedef struct  
{
  char *curpos;    /**< ��ǰָ��λ��*/
  char *endpos;    /**< βָ��λ��*/
}mvformatter_buff_t;

/**
 * @name �����ַ����������궨��
 * @{
 */
#define m_isalnum(c) (isalnum(((unsigned char)(c))))       /**< �Ƿ���...*/
#define m_isalpha(c) (isalpha(((unsigned char)(c))))       /**< �Ƿ���...*/
#define m_iscntrl(c) (iscntrl(((unsigned char)(c))))       /**< �Ƿ���...*/
#define m_isdigit(c) (isdigit(((unsigned char)(c))))       /**< �Ƿ�������(0-9)*/
#define m_isgraph(c) (isgraph(((unsigned char)(c))))       /**< �Ƿ���...*/
#define m_islower(c) (islower(((unsigned char)(c))))       /**< �Ƿ���...*/

/** @} */

/** GetStrError: ��ȡ������Ϣ
 *  @param err �������
 *  @return �ɹ����ش�����Ϣ��
 */
char *GetStrError(int err);

__END_DECLS


/** @} */


#endif
