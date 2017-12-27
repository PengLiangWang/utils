
/**
 * @file datetime.h
 * @brief  ����ʱ�䴦��
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-9
 */


#ifndef _TTSYS_DATETIME_H_
#define _TTSYS_DATETIME_H_

#include "ttdef.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_DATETIME ����ʱ�䴦��
 * @ingroup TTSYS
 * @{
 */

/**
 * ����ʱ��ṹ����
 */
typedef struct TDateTime
{
  short   h;         /**< Сʱ */
  short   min;       /**< ���� */
  short   s;         /**< �� */
  char    hms1[7];   /**< hhmmss */
  char    hms2[9];   /**< hh:mm:ss */
  char    hms3[13];  /**< hhʱmm��ss�� */
  short   y;         /**< �� */
  short   mon;       /**< �� */
  short   d;         /**< �� */
  char    ymd1[9];   /**< yyyymmdd */
  char    ymd2[11];  /**< yyyy/mm/dd */
  char    ymd3[15];  /**< yyyy��mm��dd�� */
  char    mdy1[9];   /**< mmddyyyy */
  char    mdy2[11];  /**< mm/dd/yyyy */
  char    time[15];  /**< YYYYMMDDHHMMSS */
}TDateTime;



/** ��ȡ��ǰ���ں�ʱ��
 *  @param datetime ������ں�ʱ��Ĵ洢��
 *  @return void
 */
void GetCurrentDateTime(TDateTime *datetime);

/** ���ݵ�ǰ�����ڻ�ȡ��һ������
 *  @param beginDate ��ǰ������(���ڸ�ʽ����Ϊ YYYYMMDD )
 *  @param nextDate ��һ������(���ڵĸ�ʽΪ YYYYMMDD )
 *  @return void
 */
void GetNextDate(const char *beginDate,char *nextDate);

/** ���ݵ�ǰ�����ڻ�ȡ��һ������
 *  @param beginDate ��ǰ������(���ڸ�ʽ����Ϊ YYYYMMDD )
 *  @param prevDate ��һ������(���ڵĸ�ʽΪ YYYYMMDD )
 *  @return void
 */
void GetPrevDate(const char *beginDate,char *prevDate);


/** @} */

__END_DECLS


#endif

