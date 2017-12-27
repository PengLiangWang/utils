
/**
 * @file xlate.h
 * @brief ����ת���������
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-9
 */

#ifndef _TTSYS_XLATE_H_
#define _TTSYS_XLATE_H_

#include "ttdef.h"
#include "memory.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_XLATE ����ת���������
 * @ingroup TTSYS
 * @{
 */


/**
 * ����������
 */

typedef struct TXlate  TXlate;

/** �򿪽�����
 *  @param convset ������
 *  @param to Ŀ�����
 *  @param from Դ����
 *  @param pool �ڴ��
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int XlateOpen(TXlate **convset,const char *to,const char *from,TMemPool *pool);

/** �ڲ�ʹ��
 *  @param convset ������
 *  @param onoff �رջ��Ǵ�
 *   @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int XlateSbGet(TXlate *convset, int *onoff);

/** ����ת��
 *  @param convset ������
 *  @param inbuf Դ��������
 *  @param inbytes_left ���ݴ�С
 *  @param outbuf Ŀ���������
 *  @param outbytes_left ���ݴ�С
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int XlateConvBuffer(TXlate *convset,const char *inbuf,uint32 *inbytes_left,char **outbuf,uint32 *outbytes_left);


/** �رս�����
 *  @param convset ������
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */

int XlateClose(TXlate *convset);

/** @} */

__END_DECLS


#endif

