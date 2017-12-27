
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dlist.h"

/**
 * TDListNode 链表节点结构定义
 */
struct _dlist_node_t_
{
  struct _dlist_node_t_  *next;        /**< 下一个节点 */
  struct _dlist_node_t_  *prev;        /**< 上一个节点 */
  void                   *data;        /**< 节点数据   */
  int                     size;        /**< 数据大小   */
};

/**
 * TDListNode 链表节点抽象定义
 */
typedef struct _dlist_node_t_    TDListNode;

/**
 * TDListIndex 链表索引数据结构定义
 */
struct _dlist_index_t_
{
  TDList      *list;        /**< 索引所属链表 */
  TDListNode  *this;        /**< 索引的当前节点 */
  TDListNode  *next;        /**< 索引的下一个节点 */
  TDListNode  *prev;        /**< 索引的上一个节点 */
  int          index;       /**< 索引值 */
};

/**
 * TDList 链表数据结构定义
 */
struct _dlist_t_
{
   TMemPool     *pool;         /**< 链表所属的内存池 */
   TDListNode   *head;         /**< 链表第一个节点 */
   TDListNode   *tail;         /**< 链表尾部节点 */
   int           count;        /**< 链表节点个数 */
   TDListIndex   iterator;     /**< 链表属性 */
};

int DListNew( TMemPool *pool, TDList **list)
{
   TDList *dlist;

   dlist = (TDList *)MemPoolAllocEx(pool, sizeof(TDList));
   if ( dlist == NULL )
   {
      return TTS_ENOMEM;
   }

   memset( dlist , 0 ,sizeof(TDList) );

   dlist->pool = pool;

   *list = dlist;

   return 0;
}

int DListAdd(TDList *list, void *data, int size, TMemPool *pool)
{
   TDListNode   *last;
   int           res;

   if ( list == NULL )
   {
      return TTS_EINVAL;
   }

   last = (TDListNode *)MemPoolAllocEx(list->pool, sizeof(TDListNode));
   if ( last == NULL )
   {
      return TTS_ENOMEM;
   }

   if ( pool == NULL )
   {
      pool = list->pool;
   }

   last->data = (void *)MemPoolAllocEx(pool, size);
   if ( last->data == NULL )
   {
      return TTS_ENOMEM;
   }

   memcpy(last->data, data, size);
   last->size = size;
   last->next = NULL;

   if ( list->head == NULL )
   {
      list->head = list->tail = last;
      last->prev = NULL;
   }
   else
   {
      list->tail->next = last;
      last->prev = list->tail;
      list->tail = last;
   }

   list->count++;

   return 0;

}

int DListAdd2(TDList *list, void *data, int size )
{
   TDListNode   *last;
   int           res;

   if ( list == NULL )
   {
      return TTS_EINVAL;
   }

   last = (TDListNode *)MemPoolAllocEx(list->pool, sizeof(TDListNode));
   if ( last == NULL )
   {
      return TTS_ENOMEM;
   }

   last->data = data;
   last->size = size;
   last->next = NULL;

   if ( list->head == NULL )
   {
      list->head = list->tail = last;
      last->prev = NULL;
   }
   else
   {
      list->tail->next = last;
      last->prev = list->tail;
      list->tail = last;
   }

   list->count++;

   return 0;

}

TDListIndex *DListNext(TDListIndex *li)
{
  if ( li->list->count == 0 )
  {
     return NULL;
  }

  li->this = li->next;

  if ( li->this == NULL )
  {
    if ( ( li->index > li->list->count )  || ( li->index != 0 ) )
    {
      return NULL;
    }
    else
    {
      li->this = li->list->head;
    }
  }

  li->next = li->this->next;

  li->index++;

  return li;
  
}

TDListIndex  *DListFirst( TDList *list )
{
   TDListIndex *li;

   if ( list == NULL )
   {
      return NULL;
   }

   li = (TDListIndex *)&list->iterator;
   li->list  = list;
   li->index = 0;
   li->this = li->next = li->prev = NULL;

   return DListNext( li );
}

void DListThis( TDListIndex *li,void **data )
{
  if ( data != NULL )
  {
    *data = (void *)li->this->data;
  }
}

int DListForeach(TDList *list, int (*foreachFunc)(void *data))
{
  TDListIndex     *idx;
  void            *data;
  int              res;

  for( idx = DListFirst(list); idx != NULL ; idx = DListNext(idx) )
  {
    DListThis(idx, (void **)&data);
    res = (*foreachFunc)(data);
    if ( res != 0 )
    {
      return res;
    }
  }

  return 0;
}

int DListForeach2(TDList *list, int (*foreachFunc2)(void *data,void *data2), void *data2)
{
   TDListIndex     *idx;
   void            *data;
   int              res;

   for( idx = DListFirst(list); idx != NULL ; idx = DListNext(idx) )
   {
      DListThis(idx, (void **)&data);
      res = (*foreachFunc2)(data, data2);
      if ( res == 0 )
      {
         memcpy(data2, data, idx->this->size) ;
         return res ;
      }
   }

   return res ;
}


