
/**
 * @file list.h
 * @brief ���������㷨�������ϵ����ݽ��������û��Զ��塣
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-8
 */

#ifndef _TTSYS_LIST_H_
#define _TTSYS_LIST_H_

#include "ttdef.h"

__BEGIN_DECLS


/**
 * @defgroup TTSYS_LIST ˫������
 * @brief ���������㷨�������ϵ����ݽ��������û��Զ��塣
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
 * TListNode ����ڵ�ṹ����
 */
struct _list_node_t_
{
  struct _list_node_t_  *next;        /**< ��һ���ڵ� */
  struct _list_node_t_  *prev;        /**< ��һ���ڵ� */
  void                  *data;        /**< �ڵ����� */
};

/**
 * TListNode ����ڵ������
 */
typedef struct _list_node_t_    TListNode;


/**
 * TList �������ݽṹ������
 */
typedef struct _list_t_         TList;


/**
 * TListIndex �����������ݽṹ������
 */
typedef struct _list_index_t_   TListIndex;

/**
 * TListIndex �����������ݽṹ����
 */
struct _list_index_t_
{
  TList     *list;        /**< ������������ */
  TListNode *this;        /**< �����ĵ�ǰ�ڵ� */
  TListNode *next;        /**< ��������һ���ڵ� */
  TListNode *prev;        /**< ��������һ���ڵ� */
  int        index;       /**< ����ֵ */
};

/**
 * TList �������ݽṹ����
 */
struct _list_t_
{
  TListNode   *head;         /**< �����һ���ڵ� */
  TListNode   *tail;         /**< ����β���ڵ� */
  int          count;        /**< ����ڵ���� */
  int          flags;        /**< �������� */
  TListIndex   iterator;     /**< �������� */
};

/** @} */

/**
 * @name ����������
 * @{
 */

/**
 * �����������Զ���
 */

enum LIST_REPEATABLE
{
  REPEATABLE = 1,            /**< ���ݿ��ظ� */
  NOT_REPEATABLE = 2         /**< ���ݲ����ظ� */
};

/**
 * �������������
 */
enum LIST_FOREATCH
{
  LIST_NEXT = 4,             /**< ���������� */
  LIST_PREV = 8              /**< ����ǰ����� */
};

/** @} */

/** ����List
 *  @param list ��Ҫ����������
 *  @return ���ش���������
 */
TList *ListNew( TList *list);

/** �����������ز���
 *  @param list ��Ҫ���õ�����
 *  @param flags ��ر�ʶ
 *  @remark flags ��REPEATABLE,NOT_REPEATABLE
 */
void ListSetFlag( TList *list , int flags );

/** ���������������
 *  @param list ����
 *  @param data ����
 *  @param addFunc �ص�����,��������ʱ��Ҫ���Ĳ���
 *  @return ����TTS_SUCCESS�ɹ�
 *  @remark �����������ݿ��ظ�������ú���ListSetFlag�����ظ���ʶ
 */
int ListAdd(TList *list,void *data,int (*addFunc)(void *data));

/** �������в�������
 *  @param list ����
 *  @param data ����
 *  @param addFunc �ص�����,��������ʱ��Ҫ���Ĳ���
 *  @param point �����ݲ��뵽������ǰ,�� point Ϊ�գ�����뵽�����ͷ��
 *  @return ����TTS_SUCCESS�ɹ�
 *  @remark �����������ݿ��ظ�������ú���ListSetFlag�����ظ���ʶ
 */
int ListInsert(TList *list,void *data,int (*addFunc)(void *data),void *point);

/** ɾ�������е�����
 *  @param list ��Ҫɾ��������
 *  @param data ��Ҫɾ��������
 *  @param delFunc �ص�����,ɾ������ʱ��Ҫ���Ĳ���
 *  @return ����TTS_SUCCESS�ɹ�
 */
int ListDelete(TList *list,void *data,int (*delFunc) (void *data) );

/** ɾ�������е�����
 *  @param list ��Ҫɾ��������
 *  @param data ��Ҫɾ��������
 *  @param delFunc �ص�����,ɾ������ʱ��Ҫ���Ĳ���
 *  @param compressFunc ����Ԫ�صĻص�����
 *  @return ����TTS_SUCCESS�ɹ�
 */
int ListDelete2(TList *list,void *data,int (*delFunc) (void *data),int (*compressFunc)(void *data,void *data2) );

/** ��������е�����
 *  @param list ��Ҫ���������
 *  @param clearFunc �ص�����,�������ʱ��Ҫ���Ĳ���
 *  @return ����TTS_SUCCESS�ɹ�
 */
int ListClear(TList *list,int (*clearFunc) (void *data) );

/** ��ȡ����ĵ�һ���ݵ���������
 *  @param list ���Ӧ������
 *  @return �������Ӧ����������
 */
TListIndex *ListFirst(TList *list);

/** ���ݵ�ǰ��������ȡ�������һ������
 *  @param li ���Ӧ������
 *  @return �������Ӧ��������������û���򷵻� NULL
 */
TListIndex *ListNext(TListIndex *li);

/** ListLast: ��ȡ��������һ�����ݵ���������
 *  @param list ���Ӧ������
 *  @return �������Ӧ����������
 */
TListIndex *ListLast(TList *list);

/** ���ݵ�ǰ��������ȡ�����ǰһ������
 *  @param li ���Ӧ������
 *  @return �������Ӧ��������������û���򷵻� NULL
 */
TListIndex *ListPrev(TListIndex *li);

/** ��������������ȡ���Ӧ������
 *  @param li ���Ӧ����������
 *  @param data ������ݵ�ָ��
 */
void ListThis(TListIndex *li,void **data);

/** �������ݵı���
 *  @param list ���Ӧ������
 *  @param foreachFunc �ص�����������������������Ĳ���
 *  @param action �����������
 *  @return ����TTS_SUCCESS�ɹ�
 *  @remark �����������ݵķ����ͨ�����ú���ListSetFlag���ñ�������
 */
int ListForeach(TList *list,int (*foreachFunc)(void *data),int action);

/** �������ݵı���
 *  @param list ���Ӧ������
 *  @param foreachFunc2 �ص�����������������������Ĳ���
 *  @param data2 �ص���������Ĳ���
 *  @param action �����������
 *  @return ����TTS_SUCCESS�ɹ�
 *  @remark �����������ݵķ����ͨ�����ú���ListSetFlag���ñ�������
 */
int ListForeach2(TList *list, int (*foreachFunc2)(void *data,void *data2),void *data2,int action);

/** ��ȡ�����е�Ԫ�ظ���
 *  @param list ���Ӧ������
 *  @return ����������Ԫ�ظ���
 */
int ListGetCount(TList *list);


/** @} */

__END_DECLS


#endif
