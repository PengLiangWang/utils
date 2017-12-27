#include <stdio.h>
#include "hlist.h"


#define HASH_ASSERT(exp,rv)       \
  {                               \
    if (!(exp))                   \
    {                             \
      return rv;                  \
    }                             \
  }

int HListNew(HList *hList,GetHashValueFunc GetHashValue,CompressFunc compress,int  maxHashValue)
{
  int    i;
  TList *list;
  
  HASH_ASSERT(GetHashValue != NULL,TTS_ENULL);
  HASH_ASSERT(compress != NULL,TTS_ENULL);
  HASH_ASSERT(maxHashValue > 0,TTS_EINVAL);

  hList->GetHashValue = GetHashValue;
  hList->compress = compress;
  hList->maxHashValue = maxHashValue;
    
  hList->itemList = (TList *)malloc(hList->maxHashValue * sizeof(TList));
  if (hList->itemList == NULL)
  {
    return TTS_ENOMEM;
  }

  for(i = 0; i < hList->maxHashValue; ++i)
  {
    ListNew(&hList->itemList[i]);
  }

  return TTS_SUCCESS;
}

int HListResize(HList *hList, int maxHashValue)
{
  int  i;
  
  HASH_ASSERT(maxHashValue > 0,TTS_EINVAL);

  hList->itemList = (TList *)realloc(hList->itemList,maxHashValue * sizeof(TList));
  if (hList->itemList == NULL)
  {
    return TTS_ENOMEM;
  }

  if(hList->maxHashValue < maxHashValue)
  {
    for(i = hList->maxHashValue;i < maxHashValue; ++i)
    {
      ListNew(&hList->itemList[i]);
    }
  }
  hList->maxHashValue = maxHashValue;

  return TTS_SUCCESS;

}

int HListAdd(HList *hList,void *userData)
{
  int       hashValue;

  HASH_ASSERT(hList != NULL,TTS_ENULL);
  HASH_ASSERT(hList->itemList != NULL,TTS_ENULL);

  hashValue = hList->GetHashValue(userData,hList->maxHashValue);

  HASH_ASSERT(hashValue >= 0,TTS_EINVAL);
  HASH_ASSERT(hashValue < hList->maxHashValue,TTS_EINVAL);
  
  return ListAdd(&hList->itemList[hashValue],userData,NULL);
}

int HListDelete(HList *hList,void *userData,int (*delFunc)(void *userData))
{
  int  hashValue;

  HASH_ASSERT(hList!= NULL,TTS_ENULL);
  HASH_ASSERT(hList->itemList != NULL,TTS_ENULL);

  hashValue = hList->GetHashValue(userData,hList->maxHashValue);
  HASH_ASSERT(hashValue >= 0,TTS_EINVAL);
  HASH_ASSERT(hashValue < hList->maxHashValue,TTS_EINVAL);
  return ListDelete(&hList->itemList[hashValue],userData,delFunc);
}

int HListFind(HList *hList,void *indexData,void **userDataP)
{
  int              hashValue;
  TList           *list;
  TListIndex      *index;
  void            *userData;

  HASH_ASSERT(hList != NULL,TTS_ENULL);
  HASH_ASSERT(hList->itemList != NULL,TTS_ENULL);

  hashValue = hList->GetHashValue(indexData,hList->maxHashValue);
  HASH_ASSERT(hashValue >= 0,TTS_EINVAL);
  HASH_ASSERT(hashValue < hList->maxHashValue,TTS_EINVAL);

  HASH_ASSERT(indexData != NULL,TTS_ENULL);
  HASH_ASSERT(userDataP != NULL,TTS_ENULL);


  list = &hList->itemList[hashValue];

  for ( index = ListFirst(list);index != NULL ;index = ListNext(index) )
  {
    ListThis(index,(void **)&userData);
    if ((hList->compress)(indexData,userData) == 0)
    {
      *userDataP = userData;
      return TTS_SUCCESS;
    }
  }

  return TTS_NOTFOUND;
}

int HListFind2(HList *hList,void *indexData,void **userDataP,CompressFunc compress)
{
  int              hashValue;
  TListIndex      *index;
  TList           *list;
  void            *userData;

  HASH_ASSERT(hList != NULL,TTS_ENULL);
  HASH_ASSERT(hList->itemList != NULL,TTS_ENULL);

  hashValue = hList->GetHashValue(indexData,hList->maxHashValue);
  HASH_ASSERT(hashValue >= 0,TTS_ENULL);
  HASH_ASSERT(hashValue < hList->maxHashValue,TTS_ENULL);

  HASH_ASSERT(indexData != NULL,TTS_ENULL);
  HASH_ASSERT(userDataP != NULL,TTS_ENULL);

  list = &hList->itemList[hashValue];

  for ( index = ListFirst(list);index != NULL ;index = ListNext(index) )
  {
    ListThis(index,(void **)&userData);
    if ((*compress)(indexData,userData) == 0 )
    {
      *userDataP = userData;
      return TTS_SUCCESS;
    }
  }
  return TTS_NOTFOUND;
}

int HListForeach(HList *hList,int (*foreachFunc)(void *userdata))
{
  int        rv;
  void           *userData;
  TList          *list;
  int             i;

  HASH_ASSERT(hList!=NULL,TTS_ENULL);
  HASH_ASSERT(hList->itemList!=NULL,TTS_ENULL);


  for(i = 0 ; i < hList->maxHashValue; ++i)
  {
    
    list = &hList->itemList[i];

    rv = ListForeach(list,foreachFunc,LIST_NEXT);
    if (rv != TTS_SUCCESS)
    {
      return rv;
    }
  }

  return TTS_SUCCESS;

}

int HListClean(HList *hList,int (*delFunc)(void *userData))
{
  int rv;
  int      i;

  for(i = 0; i < hList->maxHashValue; ++i)
  {
    rv = ListClear(&hList->itemList[i],delFunc);
    if( rv != TTS_SUCCESS)
    {
      return rv;
    }
  }

  return TTS_SUCCESS;
}

int HListFree(HList *hList,int (*delFunc)(void *userData))
{
  int rv;
  int      i;

  for(i = 0; i < hList->maxHashValue; ++i)
  {
    rv = ListClear(&hList->itemList[i],delFunc);
    if( rv != TTS_SUCCESS)
    {
      return rv;
    }
  }

  if(hList->itemList)
  {
    free(hList->itemList);
  }

  return TTS_SUCCESS;
}

uint32 defaultGetHashValue(uchar *key,int length)
{
 register unsigned int nr = 1, nr2 = 4;

 while (length--)
 {
   nr  ^= ( ( (nr & 63) + nr2) * ( (unsigned int) (unsigned char)*key++) ) + (nr << 8);
   nr2 += 3;
 }

 return nr;

}



