
/**
 * @file datetime.h
 * @brief  日期时间处理
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-9
 */


#ifndef _TTSYS_DATETIME_H_
#define _TTSYS_DATETIME_H_

#include "ttdef.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_DATETIME 日期时间处理
 * @ingroup TTSYS
 * @{
 */

/**
 * 日期时间结构定义
 */
typedef struct TDateTime
{
  short   h;         /**< 小时 */
  short   min;       /**< 分钟 */
  short   s;         /**< 秒 */
  char    hms1[7];   /**< hhmmss */
  char    hms2[9];   /**< hh:mm:ss */
  char    hms3[13];  /**< hh时mm分ss秒 */
  short   y;         /**< 年 */
  short   mon;       /**< 月 */
  short   d;         /**< 日 */
  char    ymd1[9];   /**< yyyymmdd */
  char    ymd2[11];  /**< yyyy/mm/dd */
  char    ymd3[15];  /**< yyyy年mm月dd日 */
  char    mdy1[9];   /**< mmddyyyy */
  char    mdy2[11];  /**< mm/dd/yyyy */
  char    time[15];  /**< YYYYMMDDHHMMSS */
}TDateTime;



/** 获取当前日期和时间
 *  @param datetime 存放日期和时间的存储区
 *  @return void
 */
void GetCurrentDateTime(TDateTime *datetime);

/** 根据当前的日期获取下一个日期
 *  @param beginDate 当前的日期(日期格式必须为 YYYYMMDD )
 *  @param nextDate 下一个日期(日期的格式为 YYYYMMDD )
 *  @return void
 */
void GetNextDate(const char *beginDate,char *nextDate);

/** 根据当前的日期获取上一个日期
 *  @param beginDate 当前的日期(日期格式必须为 YYYYMMDD )
 *  @param prevDate 上一个日期(日期的格式为 YYYYMMDD )
 *  @return void
 */
void GetPrevDate(const char *beginDate,char *prevDate);


/** @} */

__END_DECLS


#endif

