#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"


TList *ListNew( TList *list)
{
  if ( list == NULL )
  {
    list = (TList *) malloc ( sizeof( TList ) );
  }

  memset( list , 0 ,sizeof(TList) );

  if ( list != NULL )
  {
    list->head  = list->tail = NULL;
    list->count = 0;
    list->flags = NOT_REPEATABLE;
  }

  return list;
}

void ListSetFlag( TList *list , int flag )
{
  if ( list != NULL )
  {
    list->flags = flag;
  }
}

int ListAdd(TList *list,void *data,int (*addFunc)(void *data))
{
  TListNode   *p;
  int          rv;

  if ( list == NULL )
  {
    return TTS_EINVAL;
  }

  if ( list->flags == NOT_REPEATABLE )
  {
    p = list->head;
    while ( p != NULL )
    {
      if ( p->data == data )
      {
        return TTS_EXIST;
      }
      p = p->next;
    }
  }

  if ( addFunc != NULL )
  {
    rv = (*addFunc)(data);
    if ( rv != 0 )
    {
      return rv;
    }
  }

  p = (TListNode *)malloc(sizeof(TListNode));
  if ( p == NULL )
  {
    return TTS_ENOMEM;
  }

  p->data = data;
  p->next = NULL;

  if ( list->head == NULL )
  {
    list->head = list->tail = p;
    p->prev = NULL;
  }
  else
  {
    list->tail->next = p;
    p->prev = list->tail;
    list->tail = p;
  }

  list->count++;

  return 0;
}


int ListInsert(TList *list,void *data,int (*addFunc)(void *data),void *point)
{
  TListNode   *p;
  TListNode   *pNode;
  int          rv;

  if ( list == NULL )
  {
    return TTS_EINVAL;
  }

  if ( list->count == 0 )
  {
    return ListAdd(list,data,addFunc);
  }

  if ( point == NULL )
  {
    point = list->head->data;
  }

  rv = TTS_NOTFOUND;

  if ( list->flags == NOT_REPEATABLE )
  {
    p = list->head;

    while ( p != NULL )
    {
      if ( p->data == data )
      {
        return TTS_EXIST;
      }
      else if ( p->data == point)
      {
        rv = TTS_SUCCESS;
        pNode = p;
      }
      p = p->next;
    }
  }
  else
  {
    p = list->head;

    while ( p != NULL )
    {
      if ( p->data == point)
      {
        rv = TTS_SUCCESS;
        pNode = p;
      }
      p = p->next;
    }
  }

  if ( rv != TTS_SUCCESS )
  {
    return TTS_NOTFOUND;
  }

  if ( addFunc != NULL )
  {
    rv = (*addFunc)(data);
    if ( rv != 0 )
    {
      return rv;
    }
  }

  p = (TListNode *)malloc(sizeof(TListNode));
  if ( p == NULL )
  {
    return TTS_ENOMEM;
  }

  p->data = data;
  p->next = NULL;
  p->prev = NULL;

  if ( point == list->head->data )
  {
    list->head->prev = p;
    p->next = list->head;
    list->head = p;
  }
  else
  {
    p->next = pNode;
    p->prev = pNode->prev;
    pNode->prev->next = p;
    pNode->prev = p;
  }
  
  list->count++;

  return 0;
}


int ListDelete(TList *list,void *data,int (*delFunc) (void *data) )
{
  TListNode *p;
  int   rv;

  p = list->head;

  while ( p != NULL )
  {
    if ( p->data != data )
    {
      p = p->next;
    }
    else
    {
      if ( delFunc != NULL )
      {
        rv = (*delFunc)(data);
        if ( rv != TTS_SUCCESS )
        {
          return rv;
        }
      }
      if ( p->next == NULL )
      {
        p->prev->next = NULL;
        list->tail = p->prev;
      }
      else if ( p->prev == NULL )
      {
        list->head = p->next;
        list->head->prev = NULL;
      }
      else
      {
        p->prev->next = p->next;
        p->next->prev = p->prev;
      }

      free(p);
      list->count--;

      return TTS_SUCCESS;
    }
  }
}

int ListClear(TList *list,int (*clearFunc) (void *data) )
{
  TListNode *node,*node2;
  int        rv;

  node = list->head;
  
  while ( node != NULL )
  {
    node2 = node->next;

    if ( clearFunc != NULL )
    {
      rv = (*clearFunc)(node->data);
      if ( rv != 0 )
      {
        node->prev = NULL;    
        list->head = node;
        return rv;
      } 
    }

    free(node);

    list->count--;

    node = node2;   
  }

  list->head = list->tail = NULL;

  return 0;
}


TListIndex *ListNext(TListIndex *li)
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


TListIndex *ListPrev( TListIndex *li )
{
  if ( li->list->count == 0 )
  {
    return NULL;
  }
  
  li->this = li->prev;

  if ( li->this == NULL )
  {
    if ( ( li->index > li->list->count ) || ( li->index != 0 ) )
    {
      return NULL;
    }
    else
    {
      li->this =  li->list->tail;
    }
  }

  li->prev = li->this->prev;
  
  li->index++;

  return li;
}

TListIndex  *ListFirst( TList *list )
{
  TListIndex *li;

  if ( list == NULL )
  {
    return NULL;
  }

  li = (TListIndex *)&list->iterator;
  li->list  = list;
  li->index = 0;
  li->this = li->next = li->prev = NULL;

  return ListNext( li );
}


TListIndex *ListLast(TList *list)
{
  TListIndex *li;
  
  if ( list == NULL )
  {
    return NULL;
  }

  li = (TListIndex *)&list->iterator;
  li->list  = list;
  li->index = 0;
  li->this = li->next = li->prev = NULL;

  return ListPrev(li);
}

void ListThis( TListIndex *li,void **data )
{
  if ( data != NULL )
  {
    *data = (void *)li->this->data;
  }
}

typedef TListIndex * (*StepFunc)(TListIndex *idx);
typedef TListIndex * (*StepFirstFunc)(TList *list);

int ListForeach(TList *list,int (*foreachFunc)(void *data),int action)
{
  TListIndex     *idx;
  void           *data;
  StepFirstFunc   func1;
  StepFunc        func2;
  int             rv;

  if( action == LIST_NEXT )
  {
    func1 = ListFirst;
    func2 = ListNext;
  }
  else
  {
    func1 = ListLast;
    func2 = ListPrev;
  }

  for( idx = (*func1)(list); idx != NULL ; idx = (*func2)(idx) )
  {
    ListThis(idx,(void **)&data);
    rv = (*foreachFunc)(data);
    if ( rv != 0 )
    {
      return rv;
    }
  }

  return 0;
}

int ListForeach2(TList *list, int (*foreachFunc2)(void *data,void *data2),void *data2,int action)
{
  TListIndex   *idx;
  void         *data;
  StepFirstFunc func1;
  StepFunc      func2;
  int      rv;
  
  if( action == LIST_NEXT )
  {
    func1 = ListFirst;
    func2 = ListNext;
  }
  else 
  {
    func1 = ListLast;
    func2 = ListPrev;
  }

  for ( idx = (*func1)(list);idx != NULL ; idx = (*func2)(idx) )
  {
    ListThis(idx,(void **)&data);
    rv = (*foreachFunc2)(data,data2);
    if ( rv != 0 )
    {
      return rv;
    }
  }

  return 0;
}

int ListGetCount(TList *list)
{
  return list->count;
}

int ListDelete2(TList *list,void *data,int (*delFunc) (void *data),int (*compressFunc)(void *data,void *data2) )
{
  TListNode    *p;
  TListIndex   *idx;
  void         *data2;
  int      rv;

  for ( idx = ListFirst(list);idx != NULL ; idx = ListNext(idx) )
  {
    ListThis(idx,(void **)&data2);
    rv = (*compressFunc)(data,data2);
    if ( rv == TTS_SUCCESS )
    {
      p = idx->this;
      break;
    }
  }

  if ( p->next == NULL )
  {
    p->prev->next = NULL;
    list->tail = p->prev;
  }
  else if ( p->prev == NULL )
  {
    list->head = p->next;
    list->head->prev = NULL;
  }
  else
  {
    p->prev->next = p->next;
    p->next->prev = p->prev;
  }

  free(p);

  list->count--;

  return TTS_SUCCESS;

}
