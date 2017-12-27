#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "datapool.h"

enum
{
  DATA_ELT_NULL = 0,     /* 变量池元素值为NULL         */
  DATA_ELV_VAL  = 1,     /* 变量池元素有值             */
  DATA_ELT_NAME = 32,    /* 变量池数据名称长度         */
  DATA_MAX_ELTS = 512    /* 变量池中元素的最大个数定义 */
};

struct t_data_element
{
  int     vid;                       /* ID 很重要     */
  char    name[DATA_ELT_NAME];       /* NAME 对应关系 */
  int     type;                      /* 类型''''''    */
  int     len;                       /* 大小          */
  int     tag;                       /* 赋值标记      */
  char    sval[DATA_ELT_SIZE];       /* STRING VAL    */
  double  dval;                      /* DOUBLE VAL    */
  long    lval;                      /* LONG VAL      */
  int     index;                     /* 检索索引,初始为0 */
};

typedef struct t_data_element  TDataElement;

struct t_data_pool_index
{
  TDataPool  *pool;   /**< */
  int         index;
};

struct t_data_pool
{
  int            numOfElts;
  int            tag;
  TDataElement   data[DATA_MAX_ELTS+1];
  TDataElement  *elts[DATA_MAX_ELTS+1];
  TDataPoolIndex iterator;
};

#define  VAR_ASSERT(EXP,ERR) \
         if (  ( EXP ) )     \
         {                   \
           return ERR;       \
         }


void DataPoolEnableUpdate(TDataPool *dataPool)
{
   dataPool->tag = 0;
}

void DataPoolDisableUpdate(TDataPool *dataPool)
{
   dataPool->tag = 1;
}

int  DataPoolAlloc(TMemPool *memPool,TDataPool **pool)
{

  TDataPool *newPool;

  newPool = (TDataPool *)MemPoolAllocEx(memPool,sizeof(TDataPool));
  if ( newPool == NULL )
  {
    return TTS_ENOMEM;
  }
  memset(newPool,0,sizeof(TDataPool));
  newPool->numOfElts = 0;
  newPool->tag = 0;
  *pool = newPool;
  return TTS_SUCCESS;

}

int  DataPoolClear(TDataPool *pool)
{
  if ( pool == NULL )
  {
    return TTS_ENULL;
  }

  memset(pool,0,sizeof(TDataPool));
  pool->numOfElts = 0;
  return TTS_SUCCESS;
}

int  DataPoolIsExist(TDataPool *pool,int fldId)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL);
  if ( pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    return TTS_SUCCESS;
  }
  return TTS_NOTFOUND;
}


int  DataPoolGetElement(TDataPool *pool,int fldId,char **value,int *len)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL );

  if ( pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    switch (pool->data[fldId].type)
    {
      case DATA_STRING:
      case DATA_HEX:
        break;
      case DATA_LONG:
        if ( pool->data[fldId].tag != (DATA_LONG | DATA_STRING) )
        {
          memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
          sprintf(pool->data[fldId].sval,"%012ld",pool->data[fldId].lval);
          pool->data[fldId].len = 12;
          pool->data[fldId].tag |= DATA_STRING;
        }
        break;
      case DATA_DOUBLE:
        if ( pool->data[fldId].tag != (DATA_DOUBLE | DATA_STRING) )
        {
          memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
          sprintf(pool->data[fldId].sval,"%016.4f",pool->data[fldId].dval);
          pool->data[fldId].len = 16;
          pool->data[fldId].tag |= DATA_STRING;
        }
        break;
      default:
        return TTS_EINVAL;
    }
    *value = pool->data[fldId].sval;
    *len   = pool->data[fldId].len;
    return TTS_SUCCESS;
  }


  return TTS_NOTFOUND;
}


int  DataPoolPutElement(TDataPool *pool,int fldId,int fldType,char *value,int len)
{
  VAR_ASSERT( ( value == NULL ) && (fldId > DATA_MAX_ELTS), TTS_EINVAL );

  if ( pool->tag && pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    return TTS_SUCCESS;
  }

  pool->data[fldId].vid = fldId;
  pool->data[fldId].type = pool->data[fldId].tag = fldType;
  memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
  
  switch (fldType)
  {
    case DATA_STRING:
      memcpy(pool->data[fldId].sval,value,len);
      pool->data[fldId].len = len;
      break;
    case DATA_HEX:
      memcpy(pool->data[fldId].sval,value,len);
      pool->data[fldId].len = len;
      pool->data[fldId].tag = fldType | DATA_STRING;
      break;
    case DATA_LONG:
      strcpy(pool->data[fldId].sval,value);
      pool->data[fldId].len = strlen(value);
      sscanf(pool->data[fldId].sval,"%ld",&pool->data[fldId].lval);
      pool->data[fldId].tag = fldType | DATA_STRING;
      break;
    case DATA_DOUBLE:
      strcpy(pool->data[fldId].sval,value);
      pool->data[fldId].len = strlen(value);
      sscanf(pool->data[fldId].sval,"%.f",&pool->data[fldId].dval);
      pool->data[fldId].tag = fldType | DATA_STRING;
      break;
    default:
      memset(&(pool->data[fldId]),0,sizeof(TDataElement));
      return TTS_EINVAL ;
  }

  
  if ( pool->data[fldId].index == 0 )
  {
    pool->data[fldId].index = ++(pool->numOfElts);
    pool->elts[pool->numOfElts - 1] = &(pool->data[fldId]);
  }
  return TTS_SUCCESS;
}


int  DataPoolGetString(TDataPool *pool,int fldId,char **value,int *len)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL );

  if ( pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    switch (pool->data[fldId].type)
    {
      case DATA_STRING:
      case DATA_HEX:
        break;
      case DATA_LONG:
        if ( pool->data[fldId].tag != (DATA_LONG | DATA_STRING) )
        {
          memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
          sprintf(pool->data[fldId].sval,"%012ld",pool->data[fldId].lval);
          pool->data[fldId].len = 12;
          pool->data[fldId].tag |= DATA_STRING;
        }
        break;
      case DATA_DOUBLE:
        if ( pool->data[fldId].tag != (DATA_DOUBLE | DATA_STRING) )
        {
          memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
          sprintf(pool->data[fldId].sval,"%016.4f",pool->data[fldId].dval);
          pool->data[fldId].len = 16;
          pool->data[fldId].tag |= DATA_STRING;
        }
        break;
      default:
        return TTS_EINVAL;
    }
    *value = pool->data[fldId].sval;
    if ( len )
    {
      *len   = pool->data[fldId].len;
    }
    return TTS_SUCCESS;
  }

  return TTS_NOTFOUND;
}

int  DataPoolGetString2(TDataPool *pool,int fldId,char *value,int *len)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL );

  if ( pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    switch (pool->data[fldId].type)
    {
      case DATA_STRING:
      case DATA_HEX:
         break;
      case DATA_LONG:
        if ( pool->data[fldId].tag != (DATA_LONG | DATA_STRING) )
        {
          memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
          sprintf(pool->data[fldId].sval,"%012ld",pool->data[fldId].lval);
          pool->data[fldId].len = 12;
          pool->data[fldId].tag |= DATA_STRING;
        }
        break;
      case DATA_DOUBLE:
        if ( pool->data[fldId].tag != (DATA_DOUBLE | DATA_STRING) )
        {
          memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
          sprintf(pool->data[fldId].sval,"%016.4f",pool->data[fldId].dval);
          pool->data[fldId].len = 16;
          pool->data[fldId].tag |= DATA_STRING;
        }
        break;
      default:
         return TTS_EINVAL;
    }
    
    if ( !len )
    {
       return TTS_EINVAL;
    }

    if ( pool->data[fldId].len < *len )
    {
       *len = pool->data[fldId].len;
    }

    memcpy(value, pool->data[fldId].sval, *len);
    
    return TTS_SUCCESS;
  }

  return TTS_NOTFOUND;
}

int  DataPoolGetLong(TDataPool *pool,int fldId,long *value)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL );

  if ( pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    switch (pool->data[fldId].type)
    {
      case DATA_LONG:
        break;
      case DATA_STRING:
        if ( pool->data[fldId].tag != (DATA_LONG | DATA_STRING) )
        {
          sscanf(pool->data[fldId].sval,"%ld",&pool->data[fldId].lval);
          pool->data[fldId].tag |= DATA_LONG;
        }
        break;
      default:
        return TTS_EINVAL;
    }
    *value = pool->data[fldId].lval;
    return TTS_SUCCESS;
  }

  return TTS_NOTFOUND;
}

int  DataPoolGetDouble(TDataPool *pool,int fldId,double *value)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL );

  if ( pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    switch (pool->data[fldId].type)
    {
      case DATA_DOUBLE:
        break;
      case DATA_STRING:
        if ( pool->data[fldId].tag != (DATA_DOUBLE | DATA_STRING) )
        {
          sscanf(pool->data[fldId].sval,"%.f",&pool->data[fldId].dval);
          pool->data[fldId].tag |= DATA_DOUBLE;
        }
        break;
      default:
        return TTS_EINVAL;
    }
    *value = pool->data[fldId].dval;
    return TTS_SUCCESS;
  }

  return TTS_NOTFOUND;
}

int  DataPoolPutString(TDataPool *pool,int fldId,char *value)
{
  VAR_ASSERT( ( value == NULL ) && (fldId > DATA_MAX_ELTS)  , TTS_EINVAL );
  
  if ( pool->tag && pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    return TTS_SUCCESS;
  }

  pool->data[fldId].vid = fldId;
  pool->data[fldId].type = pool->data[fldId].tag = DATA_STRING;
  memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
  strcpy(pool->data[fldId].sval,value);
  pool->data[fldId].len = strlen(value);
  
  if ( pool->data[fldId].index == 0 )
  {
    pool->data[fldId].index = ++(pool->numOfElts);
    pool->elts[pool->numOfElts - 1] = &(pool->data[fldId]);
  }

  return TTS_SUCCESS;
}

int  DataPoolPutLong(TDataPool *pool,int fldId,long value)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL );

  if ( pool->tag && pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    return TTS_SUCCESS;
  }

  pool->data[fldId].vid = fldId;
  pool->data[fldId].type = pool->data[fldId].tag = DATA_LONG;
  memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
  pool->data[fldId].lval = value;
  pool->data[fldId].len = sizeof(long);
  
  if ( pool->data[fldId].index == 0 )
  {
    pool->data[fldId].index = ++(pool->numOfElts);
    pool->elts[pool->numOfElts - 1] = &(pool->data[fldId]);
  }

  return TTS_SUCCESS;
}

int  DataPoolPutDouble(TDataPool *pool,int fldId,double value)
{
  VAR_ASSERT( fldId > DATA_MAX_ELTS , TTS_EINVAL );

  if ( pool->tag && pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    return TTS_SUCCESS;
  }

  pool->data[fldId].vid = fldId;
  pool->data[fldId].type = pool->data[fldId].tag = DATA_DOUBLE;
  memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
  pool->data[fldId].dval = value;
  pool->data[fldId].len = sizeof(double);
  
  if ( pool->data[fldId].index == 0 )
  {
    pool->data[fldId].index = ++(pool->numOfElts);
    pool->elts[pool->numOfElts - 1] = &(pool->data[fldId]);
  }

  return TTS_SUCCESS;
}

int  DataPoolPutHex(TDataPool *pool,int fldId,char *value,int len)
{
  VAR_ASSERT( ( value == NULL ) && (fldId > DATA_MAX_ELTS)  , TTS_EINVAL );

  if ( pool->tag && pool->data[fldId].vid == fldId && pool->data[fldId].tag )
  {
    return TTS_SUCCESS;
  }

  pool->data[fldId].vid = fldId;
  pool->data[fldId].type = pool->data[fldId].tag = DATA_HEX;
  memset(pool->data[fldId].sval,0,DATA_ELT_SIZE);
  memcpy(pool->data[fldId].sval,value,len);
  pool->data[fldId].len = len;
  
  if ( pool->data[fldId].index == 0 )
  {
    pool->data[fldId].index = ++(pool->numOfElts);
    pool->elts[pool->numOfElts - 1] = &(pool->data[fldId]);
  }

  return TTS_SUCCESS;
}

const char *DataPoolGetName(TDataPool *pool,int fldId)
{
  if ( fldId > DATA_MAX_ELTS || fldId < 1 )
  {
     return (const char*)"unkown field";
  }
  if ( pool && pool->data[fldId].vid == fldId )
  {
    return (const char *)pool->data[fldId].name;
  }
  return (const char *)"unkown field";
}


TDataPoolIndex *DataPoolFrist(TDataPool *pool)
{
  TDataPoolIndex   *iterator;
  
  if ( pool == NULL || pool->numOfElts <= 0 )
  {
    return NULL;
  }

  iterator = (TDataPoolIndex *)&pool->iterator;
  iterator->pool = pool;
  iterator->index = 0;

  return 0;
}

TDataPoolIndex * DataPoolNext(TDataPoolIndex *iterator)
{
  if ( (iterator->index + 1) > iterator->pool->numOfElts )
  {
    return NULL;
  }
  else 
  {
    iterator->index++;
    return iterator;
  }
}

void DataPoolGetThis(TDataPoolIndex *iterator, int *fldId, char **value, int *len)
{
  TDataPool *pool;

  pool = iterator->pool;

  *fldId = pool->elts[iterator->index]->vid;

  DataPoolGetElement(pool, *fldId, value, len);

  return ;
}
