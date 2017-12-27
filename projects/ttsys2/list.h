
/**
 * @file list.h
 * @brief 链表，抽象算法，链表上的数据结点可以由用户自定义。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-8
 */

#ifndef _TTSYS_LIST_H_
#define _TTSYS_LIST_H_

#include "ttdef.h"

__BEGIN_DECLS


/**
 * @defgroup TTSYS_LIST 双向链表
 * @brief 链表，抽象算法，链表上的数据结点可以由用户自定义。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-8
 * @ingroup TTSYS 
 * @{
 */

/**
 * @name 链表数据结构抽象定义
 * @{
 */

/**
 * TListNode 链表节点结构定义
 */
struct _list_node_t_
{
  struct _list_node_t_  *next;        /**< 下一个节点 */
  struct _list_node_t_  *prev;        /**< 上一个节点 */
  void                  *data;        /**< 节点数据 */
};

/**
 * TListNode 链表节点抽象定义
 */
typedef struct _list_node_t_    TListNode;


/**
 * TList 链表数据结构抽象定义
 */
typedef struct _list_t_         TList;


/**
 * TListIndex 链表索引数据结构抽象定义
 */
typedef struct _list_index_t_   TListIndex;

/**
 * TListIndex 链表索引数据结构定义
 */
struct _list_index_t_
{
  TList     *list;        /**< 索引所属链表 */
  TListNode *this;        /**< 索引的当前节点 */
  TListNode *next;        /**< 索引的下一个节点 */
  TListNode *prev;        /**< 索引的上一个节点 */
  int        index;       /**< 索引值 */
};

/**
 * TList 链表数据结构定义
 */
struct _list_t_
{
  TListNode   *head;         /**< 链表第一个节点 */
  TListNode   *tail;         /**< 链表尾部节点 */
  int          count;        /**< 链表节点个数 */
  int          flags;        /**< 链表属性 */
  TListIndex   iterator;     /**< 链表属性 */
};

/** @} */

/**
 * @name 链表常量定义
 * @{
 */

/**
 * 链表数据属性定义
 */

enum LIST_REPEATABLE
{
  REPEATABLE = 1,            /**< 数据可重复 */
  NOT_REPEATABLE = 2         /**< 数据不可重复 */
};

/**
 * 链表遍历方向定义
 */
enum LIST_FOREATCH
{
  LIST_NEXT = 4,             /**< 链表后向操作 */
  LIST_PREV = 8              /**< 链表前向操作 */
};

/** @} */

/** 创建List
 *  @param list 需要创建的链表
 *  @return 返回创建的链表
 */
TList *ListNew( TList *list);

/** 设置链表的相关参数
 *  @param list 需要设置的链表
 *  @param flags 相关标识
 *  @remark flags 有REPEATABLE,NOT_REPEATABLE
 */
void ListSetFlag( TList *list , int flags );

/** 向链表中添加数据
 *  @param list 链表
 *  @param data 数据
 *  @param addFunc 回调函数,增加数据时所要做的操作
 *  @return 返回TTS_SUCCESS成功
 *  @remark 如链表中数据可重复则需调用函数ListSetFlag设置重复标识
 */
int ListAdd(TList *list,void *data,int (*addFunc)(void *data));

/** 向链表中插入数据
 *  @param list 链表
 *  @param data 数据
 *  @param addFunc 回调函数,增加数据时所要做的操作
 *  @param point 新数据插入到此数据前,如 point 为空，则插入到链表的头部
 *  @return 返回TTS_SUCCESS成功
 *  @remark 如链表中数据可重复则需调用函数ListSetFlag设置重复标识
 */
int ListInsert(TList *list,void *data,int (*addFunc)(void *data),void *point);

/** 删除链表中的数据
 *  @param list 需要删除的链表
 *  @param data 需要删除的数据
 *  @param delFunc 回调函数,删除数据时所要做的操作
 *  @return 返回TTS_SUCCESS成功
 */
int ListDelete(TList *list,void *data,int (*delFunc) (void *data) );

/** 删除链表中的数据
 *  @param list 需要删除的链表
 *  @param data 需要删除的数据
 *  @param delFunc 回调函数,删除数据时所要做的操作
 *  @param compressFunc 查找元素的回调函数
 *  @return 返回TTS_SUCCESS成功
 */
int ListDelete2(TList *list,void *data,int (*delFunc) (void *data),int (*compressFunc)(void *data,void *data2) );

/** 清除链表中的数据
 *  @param list 需要清除的链表
 *  @param clearFunc 回调函数,清除数据时所要做的操作
 *  @return 返回TTS_SUCCESS成功
 */
int ListClear(TList *list,int (*clearFunc) (void *data) );

/** 获取链表的第一数据的链表索引
 *  @param list 相对应的链表
 *  @return 返回相对应的链表索引
 */
TListIndex *ListFirst(TList *list);

/** 根据当前的索引获取链表的下一个索引
 *  @param li 相对应的索引
 *  @return 返回相对应的链表索引，如没有则返回 NULL
 */
TListIndex *ListNext(TListIndex *li);

/** ListLast: 获取链表的最后一个数据的链表索引
 *  @param list 相对应的链表
 *  @return 返回相对应的链表索引
 */
TListIndex *ListLast(TList *list);

/** 根据当前的索引获取链表的前一个索引
 *  @param li 相对应的索引
 *  @return 返回相对应的链表索引，如没有则返回 NULL
 */
TListIndex *ListPrev(TListIndex *li);

/** 根据链表索引获取相对应的数据
 *  @param li 相对应的链表索引
 *  @param data 相关数据的指针
 */
void ListThis(TListIndex *li,void **data);

/** 链表数据的遍历
 *  @param list 相对应的链表
 *  @param foreachFunc 回调函数，遍历链表数据所需的操作
 *  @param action 链表遍历方向
 *  @return 返回TTS_SUCCESS成功
 *  @remark 遍历链表数据的方向可通过调用函数ListSetFlag设置遍历方向
 */
int ListForeach(TList *list,int (*foreachFunc)(void *data),int action);

/** 链表数据的遍历
 *  @param list 相对应的链表
 *  @param foreachFunc2 回调函数，遍历链表数据所需的操作
 *  @param data2 回调函数所需的参数
 *  @param action 链表遍历方向
 *  @return 返回TTS_SUCCESS成功
 *  @remark 遍历链表数据的方向可通过调用函数ListSetFlag设置遍历方向
 */
int ListForeach2(TList *list, int (*foreachFunc2)(void *data,void *data2),void *data2,int action);

/** 获取链表中的元素个数
 *  @param list 相对应的链表
 *  @return 返回链表中元素个数
 */
int ListGetCount(TList *list);


/** @} */

__END_DECLS


#endif
