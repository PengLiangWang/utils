
/**
 * @file bitmap.h
 * @brief 位图常用函数
 * @author tomszhou
 * @version 1.1
 * @date 2008-3-9
 */


#ifndef _TTSYS_BITMAP_H_
#define _TTSYS_BITMAP_H_

#include "ttdef.h"
#include "memory.h"


__BEGIN_DECLS

/**
 * @defgroup TTSYS_BITMAP 位图常用函数
 * @ingroup TTSYS
 * @{
 */

/** 检查位图
 *  @param bitmap 位图表
 *  @param bit   位图 
 *  @return 存在返回1,不存在返回0
 */
int CheckBitmap(uchar *bitmap, int bit);

/** 设置位图
 *  @param bitmap 位图表
 *  @param bit   位图 
 *  @return 返回0
 */
int SetBitmap(uchar *bitmap, int bit);

/** 取消位图
 *  @param bitmap 位图表
 *  @param bit   位图 
 *  @return 返回0
 */
int UnsetBitmap(uchar *bitmap, int bit);

/** @} */

__END_DECLS

#endif


