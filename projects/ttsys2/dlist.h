
/**
 * @file dlist.h
 * @brief 双向链表，抽象算法，链表上的数据结点可以由用户自定义。
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
 * @defgroup TTSYS_DLIST 双向链表(内存池)
 * @brief 双向链表，抽象算法，链表上的数据结点可以由用户自定义。
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
 * TDList 链表数据结构抽象定义
 */
typedef struct _dlist_t_         TDList;

/**
 * TDListIndex 链表索引数据结构抽象定义
 */
typedef struct _dlist_index_t_   TDListIndex;

/** @} */

/** 创建List
 *  @param pool 内存池
 *  @param list 需要创建的链表
 *  @return 返回创建的链表
 */
int DListNew( TMemPool *pool, TDList **list);


/** 向链表中添加数据
 *  @param list 链表
 *  @param data 数据
 *  @param size 数据大小
 *  @param pool 内存池
 *  @return 返回TTS_SUCCESS成功
 */
int DListAdd(TDList *list,void *data, int size, TMemPool *pool);

/** 向链表中添加数据
 *  @param list 链表
 *  @param data 数据
 *  @param size 数据大小
 *  @return 返回TTS_SUCCESS成功
 */
int DListAdd2(TDList *list,void *data, int size );

/** 获取链表的第一数据的链表索引
 *  @param list 相对应的链表
 *  @return 返回相对应的链表索引
 */
TDListIndex *DListFirst(TDList *list);

/** 根据当前的索引获取链表的下一个索引
 *  @param li 相对应的索引
 *  @return 返回相对应的链表索引，如没有则返回 NULL
 */
TDListIndex *DListNext(TDListIndex *li);


/** 根据链表索引获取相对应的数据
 *  @param li 相对应的链表索引
 *  @param data 相关数据的指针
 */
void DListThis(TDListIndex *li,void **data);

/** 链表数据的遍历
 *  @param list 相对应的链表
 *  @param foreachFunc 回调函数，遍历链表数据所需的操作
 *  @return 返回TTS_SUCCESS成功
 *  @remark 遍历链表数据的方向可通过调用函数ListSetFlag设置遍历方向
 */
int DListForeach(TDList *list,int (*foreachFunc)(void *data));

/** 链表数据的遍历
 *  @param list 相对应的链表
 *  @param foreachFunc2 回调函数，遍历链表数据所需的操作
 *  @param data2 回调函数所需的参数
 *  @return 返回TTS_SUCCESS成功
 *  @remark 遍历链表数据的方向可通过调用函数ListSetFlag设置遍历方向
 */
int DListForeach2(TDList *list, int (*foreachFunc2)(void *data,void *data2),void *data2);


/** @} */

__END_DECLS


#endif
