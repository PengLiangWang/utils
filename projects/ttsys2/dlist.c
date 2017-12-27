
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dlist.h"

/**
 * TDListNode ����ڵ�ṹ����
 */
struct _dlist_node_t_
{
  struct _dlist_node_t_  *next;        /**< ��һ���ڵ� */
  struct _dlist_node_t_  *prev;        /**< ��һ���ڵ� */
  void                   *data;        /**< �ڵ�����   */
  int                     size;        /**< ���ݴ�С   */
};

/**
 * TDListNode ����ڵ������
 */
typedef struct _dlist_node_t_    TDListNode;

/**
 * TDListIndex �����������ݽṹ����
 */
struct _dlist_index_t_
{
  TDList      *list;        /**< ������������ */
  TDListNode  *this;        /**< �����ĵ�ǰ�ڵ� */
  TDListNode  *next;        /**< ��������һ���ڵ� */
  TDListNode  *prev;        /**< ��������һ���ڵ� */
  int          index;       /**< ����ֵ */
};

/**
 * TDList �������ݽṹ����
 */
struct _dlist_t_
{
   TMemPool     *pool;         /**< �����������ڴ�� */
   TDListNode   *head;         /**< �����һ���ڵ� */
   TDListNode   *tail;         /**< ����β���ڵ� */
   int           count;        /**< ����ڵ���� */
   TDListIndex   iterator;     /**< �������� */
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


