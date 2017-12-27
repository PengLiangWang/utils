
/**
 * @file datapool.h
 * @brief  数据池组件
 * @author tomszhou
 * @version 2.0
 * @date 2013-3-9
 * @
 */

#ifndef _TTSYS_DATA_POOL_H_
#define _TTSYS_DATA_POOL_H_

#include "ttdef.h"
#include "memory.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_DATAPOOL 数据池
 * @ingroup TTSYS
 * @{
 */

/**
 *  数据池常量定义
 */
enum
{
  DATA_STRING    = 2,      /* 变量池中字符串类型   */  
  DATA_LONG      = 4,      /* 变量池中整形类型     */
  DATA_DOUBLE    = 8,      /* 变量池中浮点类型     */
  DATA_HEX       = 16,     /* 变量池中HEX类型      */
  DATA_ELT_SIZE  = 0x200,  /* 变量池数据最大的大小 */
  DATA_POOL_SIZE = 0x50000 /* 变量池的大小         */
};

/**
 * 数据池存储结构定义
 */
typedef struct t_data_pool        TDataPool;
typedef struct t_data_pool_index  TDataPoolIndex;


/** 变量池允许更新数据项
 *  @param memPool    内存池
 *  @return 无返回
 */
void DataPoolEnableUpdate(TDataPool *dataPool);

/** 变量池不允许更新数据项
 *  @param memPool    内存池
 *  @return 无返回
 */
void DataPoolDisableUpdate(TDataPool *dataPool);

/** 分配内存变量池 Alloc 
 *  @param memPool    内存池
 *  @param pool       数据池
 *  @return 成功返回TTS_SUCCESS。
 */
int  DataPoolAlloc(TMemPool *memPool,TDataPool **pool);

/** 清除内存变量池 Clear 
 *  @param pool       数据池
 *  @return 成功返回TTS_SUCCESS。
 **/
int  DataPoolClear(TDataPool *pool);

/** 判断变量池中是否存在ID的变量 
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @return 存在返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 **/
int  DataPoolIsExist(TDataPool *pool,int fldId);

/** 获取变量池中ID的值,取出的值为String, len可以为NULL 
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      变量值(String)
 *  @param len        变量大小(len可以为NULL)
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolGetElement(TDataPool *pool,int fldId,char **value,int *len);


/** 向变量池中推ID变量,value为String类型,如果是HEX类型的数据,长度len必须填写 
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param fldType    变量类型(DATA_STRING,DATA_LONG,DATA_DOUBLE,DATA_HEX);
 *  @param value      变量值(String)
 *  @param len        变量大小
 *  @return 成功返回TTS_SUCCESS
 */
int  DataPoolPutElement(TDataPool *pool,int fldId,int fldType,char *value,int len);


/** 获取变量池中ID的值,变量的类型可以为HEX
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      变量值(String)
 *  @param len        变量大小(必须填写)
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolGetString(TDataPool *pool,int fldId,char **value,int *len);

/** 获取变量池中ID的值,变量的类型可以为HEX
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      变量值(String)
 *  @param len        变量大小(必须填写)
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolGetString2(TDataPool *pool,int fldId,char *value,int *len);


/** 获取变量池中ID的值,
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      变量值(long)
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolGetLong(TDataPool *pool,int fldId,long *value);

/** 获取变量池中ID的值
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      变量值(double)
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolGetDouble(TDataPool *pool,int fldId,double *value);

/** 向变量池中推ID变量
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      变量值(类型为string)
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolPutString(TDataPool *pool,int fldId,char *value);

/** 向变量池中推ID变量
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      变量值(long)
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolPutLong(TDataPool *pool,int fldId,long value);

/** 向变量池中推ID变量
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      类型为double
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolPutDouble(TDataPool *pool,int fldId,double value);

/** 向变量池中推ID变量
 *  @param pool       数据池
 *  @param fldId      变量ID
 *  @param value      类型为HEX
 *  @param len        必须填写
 *  @return 成功返回TTS_SUCCESS,不存在返回TTS_NOTFOUND
 */
int  DataPoolPutHex(TDataPool *pool,int fldId,char *value,int len);

TDataPoolIndex *DataPoolFrist(TDataPool *pool);

TDataPoolIndex *DataPoolNext(TDataPoolIndex *iterator);

void DataPoolGetThis(TDataPoolIndex *iterator, int *fldId, char **value, int *len);


/** @} */

__END_DECLS

#endif



