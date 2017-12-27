
/**
 * @file xlate.h
 * @brief 语言转换处理组件
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
 * @defgroup TTSYS_XLATE 编码转换处理组件
 * @ingroup TTSYS
 * @{
 */


/**
 * 解析器定义
 */

typedef struct TXlate  TXlate;

/** 打开解析器
 *  @param convset 解析器
 *  @param to 目标编码
 *  @param from 源编码
 *  @param pool 内存池
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int XlateOpen(TXlate **convset,const char *to,const char *from,TMemPool *pool);

/** 内部使用
 *  @param convset 解析器
 *  @param onoff 关闭还是打开
 *   @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int XlateSbGet(TXlate *convset, int *onoff);

/** 编码转换
 *  @param convset 解析器
 *  @param inbuf 源编码数据
 *  @param inbytes_left 数据大小
 *  @param outbuf 目标编码数据
 *  @param outbytes_left 数据大小
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int XlateConvBuffer(TXlate *convset,const char *inbuf,uint32 *inbytes_left,char **outbuf,uint32 *outbytes_left);


/** 关闭解析器
 *  @param convset 解析器
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */

int XlateClose(TXlate *convset);

/** @} */

__END_DECLS


#endif

