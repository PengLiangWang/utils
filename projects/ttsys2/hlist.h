
/**
 * @file hlist.h
 * @brief 哈希链表，抽象算法，链表上的数据结点可以由用户自定义。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-18
 */


#ifndef _TTSYS_HASHLIST_H_
#define _TTSYS_HASHLIST_H_

#include "ttdef.h"
#include "list.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_HLIST 哈希链表
 * @brief 哈希链表，抽象算法，链表上的数据结点可以由用户自定义。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-18
 * @ingroup TTSYS 
 * @{
 */

/**
 * @name 哈希链表常用回调函数定义
 * @{
 */

typedef int (*GetHashValueFunc)(void *userData,int hasMaxVal);
typedef int (*CompressFunc)(void *data1,void *data2);

/** @} */

/**
 * HList 哈希链表抽象定义
 */

typedef struct HList HList;

/**
 * HList 哈希链表数据结构定义
 */
struct HList
{
  GetHashValueFunc   GetHashValue;   /**< 获取哈希值函数 */
  CompressFunc       compress;       /**< 比较函数 */
  int                maxHashValue;   /**< 哈希值 */
  TList             *itemList;       /**< 数据区 */
};


/** \fn int HListNew(HList *hList,GetHashValueFunc GetHashValue,CompressFunc compress,int  maxHashValue)
 *  \brief 创建哈希链表
 *  @param hList 需要创建的哈希链表
 *  @param GetHashValue 获取哈希值的回调函数
 *  @param compress 哈希链表中元素比较的回调函数
 *  @param maxHashValue 最大的哈希值(是个质数或者是 2^n - 1)
 *  @return 返回TTS_SUCCESS成功
 */
int HListNew(HList *hList,GetHashValueFunc GetHashValue,CompressFunc compress,int  maxHashValue);

/** HListResize: 根据哈希值重新构造哈希链表
 *  @param hList 需要重新构造的哈希链表
 *  @param maxHashValue 最大的哈希值(是个质数或者是 2^n - 1)
 *  @return 返回TTS_SUCCESS成功
 */
int HListResize(HList *hList, int maxHashValue);

/** HListAdd: 增加元素
 *  @param hList 相对应的哈希链表
 *  @param userData 需增加的元素
 *  @return 返回TTS_SUCCESS成功
 */
int HListAdd(HList *hList,void *userData);

/** HListDelete: 删除哈希链表中的元素
 *  @param hList 相对应的哈希链表
 *  @param userData 需删除的元素
 *  @param delFunc 删除元素时的回调函数
 *  @return 返回TTS_SUCCESS成功
 */
int HListDelete(HList *hList,void *userData,int (*delFunc)(void *userData));

/** HListFind: 查找哈希链表中的元素
 *  @param hList 相对应的哈希链表
 *  @param indexData 元素标识
 *  @param userDataP 找到的相对应的元素
 *  @return 返回TTS_SUCCESS成功
 */
int HListFind(HList *hList,void *indexData,void **userDataP);

/** HListFind2: 查找哈希链表中的元素
 *  @param hList 相对应的哈希链表
 *  @param indexData 元素标识
 *  @param userDataP 找到的相对应的元素
 *  @param compress 哈希链表中元素比较的回调函数
 *  @return 返回TTS_SUCCESS成功
 */
int HListFind2(HList *hList,void *indexData,void **userDataP,CompressFunc compress);

/** HListClean: 清空哈希链表中的元素
 *  @param hList 相对应的哈希链表
 *  @param delFunc 删除元素时的回调函数
 *  @return 返回TTS_SUCCESS成功
 */
int HListClean(HList *hList,int (*delFunc)(void *userData));

/** HListFree: 释放哈希链表中的元素
 *  @param hList 相对应的哈希链表
 *  @param delFunc 删除元素时的回调函数
 *  @return 返回TTS_SUCCESS成功
 */
int HListFree(HList *hList,int (*delFunc)(void *userData));

/** HListForeach: 遍历哈希链表中的数据
 *  @param hList 相对应的哈希链表
 *  @param foreachFunc 遍历时的回调函数
 *  @return 返回TTS_SUCCESS成功
 */
int HListForeach(HList *hList,int (*foreachFunc)(void *userdata));

/** defaultGetHashValue: 默认的哈希值计算函数
 *  @param key 对应的键值
 *  @param length 键值的长度
 *  @return 返回TTS_SUCCESS成功
 */
uint32 defaultGetHashValue(uchar *key,int length);


/** @} */

__END_DECLS

#endif



