
/**
 * @file datapool.h
 * @brief  ���ݳ����
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
 * @defgroup TTSYS_DATAPOOL ���ݳ�
 * @ingroup TTSYS
 * @{
 */

/**
 *  ���ݳس�������
 */
enum
{
  DATA_STRING    = 2,      /* ���������ַ�������   */  
  DATA_LONG      = 4,      /* ����������������     */
  DATA_DOUBLE    = 8,      /* �������и�������     */
  DATA_HEX       = 16,     /* ��������HEX����      */
  DATA_ELT_SIZE  = 0x200,  /* �������������Ĵ�С */
  DATA_POOL_SIZE = 0x50000 /* �����صĴ�С         */
};

/**
 * ���ݳش洢�ṹ����
 */
typedef struct t_data_pool        TDataPool;
typedef struct t_data_pool_index  TDataPoolIndex;


/** �������������������
 *  @param memPool    �ڴ��
 *  @return �޷���
 */
void DataPoolEnableUpdate(TDataPool *dataPool);

/** �����ز��������������
 *  @param memPool    �ڴ��
 *  @return �޷���
 */
void DataPoolDisableUpdate(TDataPool *dataPool);

/** �����ڴ������ Alloc 
 *  @param memPool    �ڴ��
 *  @param pool       ���ݳ�
 *  @return �ɹ�����TTS_SUCCESS��
 */
int  DataPoolAlloc(TMemPool *memPool,TDataPool **pool);

/** ����ڴ������ Clear 
 *  @param pool       ���ݳ�
 *  @return �ɹ�����TTS_SUCCESS��
 **/
int  DataPoolClear(TDataPool *pool);

/** �жϱ��������Ƿ����ID�ı��� 
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @return ���ڷ���TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 **/
int  DataPoolIsExist(TDataPool *pool,int fldId);

/** ��ȡ��������ID��ֵ,ȡ����ֵΪString, len����ΪNULL 
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ֵ(String)
 *  @param len        ������С(len����ΪNULL)
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolGetElement(TDataPool *pool,int fldId,char **value,int *len);


/** �����������ID����,valueΪString����,�����HEX���͵�����,����len������д 
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param fldType    ��������(DATA_STRING,DATA_LONG,DATA_DOUBLE,DATA_HEX);
 *  @param value      ����ֵ(String)
 *  @param len        ������С
 *  @return �ɹ�����TTS_SUCCESS
 */
int  DataPoolPutElement(TDataPool *pool,int fldId,int fldType,char *value,int len);


/** ��ȡ��������ID��ֵ,���������Ϳ���ΪHEX
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ֵ(String)
 *  @param len        ������С(������д)
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolGetString(TDataPool *pool,int fldId,char **value,int *len);

/** ��ȡ��������ID��ֵ,���������Ϳ���ΪHEX
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ֵ(String)
 *  @param len        ������С(������д)
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolGetString2(TDataPool *pool,int fldId,char *value,int *len);


/** ��ȡ��������ID��ֵ,
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ֵ(long)
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolGetLong(TDataPool *pool,int fldId,long *value);

/** ��ȡ��������ID��ֵ
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ֵ(double)
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolGetDouble(TDataPool *pool,int fldId,double *value);

/** �����������ID����
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ֵ(����Ϊstring)
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolPutString(TDataPool *pool,int fldId,char *value);

/** �����������ID����
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ֵ(long)
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolPutLong(TDataPool *pool,int fldId,long value);

/** �����������ID����
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����Ϊdouble
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolPutDouble(TDataPool *pool,int fldId,double value);

/** �����������ID����
 *  @param pool       ���ݳ�
 *  @param fldId      ����ID
 *  @param value      ����ΪHEX
 *  @param len        ������д
 *  @return �ɹ�����TTS_SUCCESS,�����ڷ���TTS_NOTFOUND
 */
int  DataPoolPutHex(TDataPool *pool,int fldId,char *value,int len);

TDataPoolIndex *DataPoolFrist(TDataPool *pool);

TDataPoolIndex *DataPoolNext(TDataPoolIndex *iterator);

void DataPoolGetThis(TDataPoolIndex *iterator, int *fldId, char **value, int *len);


/** @} */

__END_DECLS

#endif



