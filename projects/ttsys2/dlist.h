
/**
 * @file dlist.h
 * @brief ˫�����������㷨�������ϵ����ݽ��������û��Զ��塣
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-8
 */

#ifndef _TTSYS_DLIST_H_
#define _TTSYS_DLIST_H_

#include "ttdef.h"
#include "memory.h"

__BEGIN_DECLS


/**
 * @defgroup TTSYS_DLIST ˫������(�ڴ��)
 * @brief ˫�����������㷨�������ϵ����ݽ��������û��Զ��塣
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-8
 * @ingroup TTSYS 
 * @{
 */

/**
 * @name �������ݽṹ������
 * @{
 */

/**
 * TDList �������ݽṹ������
 */
typedef struct _dlist_t_         TDList;

/**
 * TDListIndex �����������ݽṹ������
 */
typedef struct _dlist_index_t_   TDListIndex;

/** @} */

/** ����List
 *  @param pool �ڴ��
 *  @param list ��Ҫ����������
 *  @return ���ش���������
 */
int DListNew( TMemPool *pool, TDList **list);


/** ���������������
 *  @param list ����
 *  @param data ����
 *  @param size ���ݴ�С
 *  @param pool �ڴ��
 *  @return ����TTS_SUCCESS�ɹ�
 */
int DListAdd(TDList *list,void *data, int size, TMemPool *pool);

/** ���������������
 *  @param list ����
 *  @param data ����
 *  @param size ���ݴ�С
 *  @return ����TTS_SUCCESS�ɹ�
 */
int DListAdd2(TDList *list,void *data, int size );

/** ��ȡ����ĵ�һ���ݵ���������
 *  @param list ���Ӧ������
 *  @return �������Ӧ����������
 */
TDListIndex *DListFirst(TDList *list);

/** ���ݵ�ǰ��������ȡ�������һ������
 *  @param li ���Ӧ������
 *  @return �������Ӧ��������������û���򷵻� NULL
 */
TDListIndex *DListNext(TDListIndex *li);


/** ��������������ȡ���Ӧ������
 *  @param li ���Ӧ����������
 *  @param data ������ݵ�ָ��
 */
void DListThis(TDListIndex *li,void **data);

/** �������ݵı���
 *  @param list ���Ӧ������
 *  @param foreachFunc �ص�����������������������Ĳ���
 *  @return ����TTS_SUCCESS�ɹ�
 *  @remark �����������ݵķ����ͨ�����ú���ListSetFlag���ñ�������
 */
int DListForeach(TDList *list,int (*foreachFunc)(void *data));

/** �������ݵı���
 *  @param list ���Ӧ������
 *  @param foreachFunc2 �ص�����������������������Ĳ���
 *  @param data2 �ص���������Ĳ���
 *  @return ����TTS_SUCCESS�ɹ�
 *  @remark �����������ݵķ����ͨ�����ú���ListSetFlag���ñ�������
 */
int DListForeach2(TDList *list, int (*foreachFunc2)(void *data,void *data2),void *data2);


/** @} */

__END_DECLS


#endif
