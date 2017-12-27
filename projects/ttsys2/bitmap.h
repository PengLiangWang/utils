
/**
 * @file bitmap.h
 * @brief λͼ���ú���
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
 * @defgroup TTSYS_BITMAP λͼ���ú���
 * @ingroup TTSYS
 * @{
 */

/** ���λͼ
 *  @param bitmap λͼ��
 *  @param bit   λͼ 
 *  @return ���ڷ���1,�����ڷ���0
 */
int CheckBitmap(uchar *bitmap, int bit);

/** ����λͼ
 *  @param bitmap λͼ��
 *  @param bit   λͼ 
 *  @return ����0
 */
int SetBitmap(uchar *bitmap, int bit);

/** ȡ��λͼ
 *  @param bitmap λͼ��
 *  @param bit   λͼ 
 *  @return ����0
 */
int UnsetBitmap(uchar *bitmap, int bit);

/** @} */

__END_DECLS

#endif


