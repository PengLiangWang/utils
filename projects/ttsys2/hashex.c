#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashex.h"

struct hashex_entry_t
{
  int         next;      /* THasheExEntry   *next */
  uint32      hash;
  int         key;       /* const char *key  */
  int         klen;
  int         val;       /* //const char *val */
  int         vlen;
  int         mlen;
};

typedef struct hashex_entry_t THasheExEntry;


struct hashex_index_t
{
  THashEx       *ht;        /* THashEx *ht*/
  int            this,next; //THasheExEntry *this,*next;
  int            index;
};


struct hashex_entry_array_t
{
  uint32           max;
  int              count;
  int              array[1];
};

typedef struct hashex_entry_array_t THasheExEntryArray;

struct hashex_t
{
  TMemPool             *pool;
  TShmemPool           *spool;
  THasheExEntryArray   *array;
  THasheExIndex         iterator;
  THashExFunc           hashFunc;
};



unsigned int HashExFuncDefault33(const char *char_key, uint32 *klen)
{
  unsigned int hash = 0;
  const unsigned char *key = (const unsigned char *)char_key;
  const unsigned char *p;
  uint32 i;

  /*
   * This is the popular `times 33' hash algorithm which is used by
   * perl and also appears in Berkeley DB. This is one of the best
   * known hash functions for strings because it is both computed
   * very fast and distributes very well.
   *
   * The originator may be Dan Bernstein but the code in Berkeley DB
   * cites Chris Torek as the source. The best citation I have found
   * is "Chris Torek, Hash function for text in C, Usenet message
   * <27038@mimsy.umd.edu> in comp.lang.c , October, 1990." in Rich
   * Salz's USENIX 1992 paper about INN which can be found at
   * <http://citeseer.nj.nec.com/salz92internetnews.html>.
   *
   * The magic of number 33, i.e. why it works better than many other
   * constants, prime or not, has never been adequately explained by
   * anyone. So I try an explanation: if one experimentally tests all
   * multipliers between 1 and 256 (as I did while writing a low-level
   * data structure library some time ago) one detects that even
   * numbers are not useable at all. The remaining 128 odd numbers
   * (except for the number 1) work more or less all equally well.
   * They all distribute in an acceptable way and this way fill a hash
   * table with an average percent of approx. 86%.
   *
   * If one compares the chi^2 values of the variants (see
   * Bob Jenkins ``Hashing Frequently Asked Questions'' at
   * http://burtleburtle.net/bob/hash/hashfaq.html for a description
   * of chi^2), the number 33 not even has the best value. But the
   * number 33 and a few other equally good numbers like 17, 31, 63,
   * 127 and 129 have nevertheless a great advantage to the remaining
   * numbers in the large set of possible multipliers: their multiply
   * operation can be replaced by a faster operation based on just one
   * shift plus either a single addition or subtraction operation. And
   * because a hash function has to both distribute good _and_ has to
   * be very fast to compute, those few numbers should be preferred.
   *
   *                  -- Ralf S. Engelschall <rse@engelschall.com>
   */

  if (*klen == HASHEX_KEY_STRING)
  {
    for (p = key; *p; p++)
    {
      hash = hash * 33 + *p;
    }
    *klen = p - key;
  }
  else
  {
    for (p = key, i = *klen; i; i--, p++)
    {
      hash = hash * 33 + *p;
    }
  }

  return hash;
}

unsigned int HashExFuncDefault63(const char *char_key, uint32 *klen)
{
  register unsigned int nr = 1, nr2 = 4,i;
  const unsigned char *key = (const unsigned char *)char_key;
  const unsigned char *p;


  if (*klen == HASHEX_KEY_STRING)
  {
    for (p = key; *p; p++)
    {
      nr ^= ( ( (nr & 63) + nr2) * ( (unsigned int) (unsigned char)*p) ) + (nr << 8);
      nr2 += 3;
    }
    *klen = p - key;
  }
  else
  {
    for ( p = key,i = *klen; i;i--,p++)
    {
      nr  ^= ( ( (nr & 63) + nr2) * ( (unsigned int) (unsigned char)*key++) ) + (nr << 8);
      nr2 += 3;
    }
  }

  return nr;
}


int HashExMake(TMemPool *pool,TShmemPool *spool,int max,THashEx **hash,int *addr)
{
  THashEx            *ht;
  THasheExEntryArray *list;
  THasheExEntry      *entry;
  int                i;
  int                rv;

  ht = (THashEx *)MemPoolAllocEx(pool,sizeof(THashEx));
  if ( ht == NULL )
  {
    return TTS_ENOMEM;
  }
  ht->pool = pool;
  ht->spool = spool;

  list = (THasheExEntryArray *)ShmemPoolAlloc(ht->spool,sizeof(int) * (max + 3));
  if ( list == NULL )
  {
    return TTS_ENOMEM;
  }

  memset(list,0,sizeof(int) * (max + 3));
  list->count = 0;
  list->max = max;
  
  entry = (THasheExEntry *)ShmemPoolAlloc(ht->spool,sizeof(THasheExEntry) * (max+1));
  if ( entry == NULL )
  {
    return TTS_ENOMEM;
  }
  memset(entry,0,sizeof(int) * (max + 1));

  for ( i = 0 ; i < max+1;i++ )
  {
    list->array[i] = ShmemGetOffset(spool,(char *)&entry[i]);
  }
  
  ht->array = list;
  ht->hashFunc = HashExFuncDefault33;

  if ( addr )
  {
    *addr = ShmemGetOffset(spool,(char *)ht->array);
  }

  *hash = ht;

  return TTS_SUCCESS;
}

int HashExMake63(TMemPool *pool,TShmemPool *spool,int max,THashEx **hash,int *addr)
{
  THashEx            *ht;
  THasheExEntryArray *list;
  THasheExEntry      *entry;
  int                i;
  int                rv;

  ht = (THashEx *)MemPoolAllocEx(pool,sizeof(THashEx));
  if ( ht == NULL )
  {
    return TTS_ENOMEM;
  }
  ht->pool = pool;
  ht->spool = spool;

  list = (THasheExEntryArray *)ShmemPoolAlloc(ht->spool,sizeof(int) * (max + 3));
  if ( list == NULL )
  {
    return TTS_ENOMEM;
  }

  memset(list,0,sizeof(int) * (max + 3));
  list->count = 0;
  list->max = max;
  
  entry = (THasheExEntry *)ShmemPoolAlloc(ht->spool,sizeof(THasheExEntry) * (max+1));
  if ( entry == NULL )
  {
    return TTS_ENOMEM;
  }
  memset(entry,0,sizeof(int) * (max + 1));

  for ( i = 0 ; i < max+1;i++ )
  {
    list->array[i] = ShmemGetOffset(spool,(char *)&entry[i]);
  }
  
  ht->array = list;
  ht->hashFunc = HashExFuncDefault63;

  if ( addr )
  {
    *addr = ShmemGetOffset(spool,(char *)ht->array);
  }

  *hash = ht;

  return TTS_SUCCESS;
}

int HashExOpen(TMemPool *pool,TShmemPool *spool,int addr,THashEx **hash)
{
  THashEx  *ht;

  ht = (THashEx *)MemPoolAllocEx(pool,sizeof(THashEx));
  if ( ht == NULL )
  {
    return TTS_ENOMEM;
  }
  ht->pool = pool;
  ht->spool = spool;
  ht->array = ( THasheExEntryArray *)ShmemPoolGetAddr(spool,addr);
  if ( ht->array == NULL )
  {
    return TTS_EINVAL;
  }
  ht->hashFunc = HashExFuncDefault33;
  *hash = ht;

  return TTS_SUCCESS;
}

int HashExOpen63(TMemPool *pool,TShmemPool *spool,int addr,THashEx **hash)
{
  THashEx  *ht;

  ht = (THashEx *)MemPoolAllocEx(pool,sizeof(THashEx));
  if ( ht == NULL )
  {
    return TTS_ENOMEM;
  }
  ht->pool = pool;
  ht->spool = spool;
  ht->array = ( THasheExEntryArray *)ShmemPoolGetAddr(spool,addr);
  if ( ht->array == NULL )
  {
    return TTS_EINVAL;
  }
  ht->hashFunc = HashExFuncDefault63;
  *hash = ht;

  return TTS_SUCCESS;
}


int HashExSet(THashEx *ht,const char *key,int klen,const void *val,int vlen)
{
  THasheExEntryArray  *list = ht->array;
  THasheExEntry       *he,*entry;
  unsigned int         hash;
  char                *pkey;
  void                *pval;

  hash = ht->hashFunc(key, &klen);
  he = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,list->array[hash & list->max]);
  if ( he == NULL )
  {
    return TTS_ENULL;
  }

  if ( he->key != 0 )
  {
    for ( ; ; )
    {
      pkey = (char *)ShmemPoolGetAddr(ht->spool,he->key);
      if ( he->hash == hash && he->klen == klen && memcmp(pkey,key,klen) == 0 )
      {
        if ( he->mlen < vlen )
        {          
          pval = (void *)ShmemPoolAlloc(ht->spool,vlen + 1);
          if ( pval == NULL )
          {
            return TTS_ENOMEM;
          }
          memcpy(pval,val,vlen);
          he->val = ShmemGetOffset(ht->spool,pval);
          he->vlen = vlen;
          he->mlen = vlen + 1;
        }
        else
        {
          pval = (void *)ShmemPoolGetAddr(ht->spool,he->val);
          memset(pval,0,he->mlen);
          memcpy(pval,val,vlen);
          he->vlen = vlen;
        }
        return TTS_SUCCESS;
      }
      if ( he->next == 0 )
      {
        entry = (THasheExEntry *)ShmemPoolAlloc(ht->spool,sizeof(THasheExEntry));
        if ( entry == NULL )
        {
          return TTS_ENOMEM;
        }
        he->next = ShmemGetOffset(ht->spool,(char *)entry);
        he = entry;
        break;
      }
      else
      {
        he = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,he->next);
      }
    }
  }

  he->next = 0;
  he->hash = hash;
  pkey = (char *)ShmemPoolAlloc(ht->spool,klen + 1);
  if ( pkey == NULL )
  {
    return TTS_ENOMEM;
  }
  memcpy(pkey,key,klen);
  he->key = ShmemGetOffset(ht->spool,pkey);
  he->klen = klen;
  pval = (void *)ShmemPoolAlloc(ht->spool,vlen + 1);
  if ( pval == NULL )
  {
    return TTS_ENOMEM;
  }
  memcpy(pval,val,vlen);
  he->vlen = vlen;
  he->mlen = vlen + 1;
  he->val = ShmemGetOffset(ht->spool,pval);
  list->count++;

  return TTS_SUCCESS;
}

int HashExGet(THashEx *ht,const char *key,int klen,void *val,int *vlen)
{
  THasheExEntryArray  *list = ht->array;
  THasheExEntry       *he;
  unsigned int         hash;
  char                *pkey;
  void                *pval;

  hash = ht->hashFunc(key, &klen);
  he = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,list->array[hash & list->max]);
  if ( he == NULL )
  {
    return TTS_ENULL;
  }

  for ( ; he != NULL && he->key != 0 ; )
  {
    pkey = (char *)ShmemPoolGetAddr(ht->spool,he->key);
    if ( he->hash == hash && he->klen == klen && memcmp(pkey,key,klen) == 0 )
    {
      pval = (void *)ShmemPoolGetAddr(ht->spool,he->val);
      if ( vlen == NULL )
      {
        memset((char*)val,0,he->vlen);
        memcpy((char*)val,pval,he->vlen);
        return TTS_SUCCESS;
      }
      else if ( *vlen <= 0 || *vlen > he->vlen)
      {
        memset((char*)val,0,he->vlen);
        memcpy((char*)val,pval,he->vlen);
        *vlen = he->vlen;
        return TTS_SUCCESS;
      }
      else
      {
        memset((char*)val,0,*vlen);
        memcpy((char*)val,pval,*vlen);
        return TTS_SUCCESS;
      }
    }
    he = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,he->next);
  }

  return TTS_NOTFOUND;
}

int HashExGet2(THashEx *ht,const char *key,int klen,void **val,int *vlen)
{
  THasheExEntryArray  *list = ht->array;
  THasheExEntry       *he;
  uint32               hash;
  char                *pkey;

  hash = ht->hashFunc(key, &klen);
  he = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,list->array[hash & list->max]);
  if ( he == NULL )
  {
    return TTS_ENULL;
  }

  for ( ; he != NULL && he->key != 0 ; )
  {
    pkey = (char *)ShmemPoolGetAddr(ht->spool,he->key);
    if ( he->hash == hash && he->klen == klen && memcmp(pkey,key,klen) == 0 )
    {
      if ( val )
      {
         *val = (void *)ShmemPoolGetAddr(ht->spool,he->val);
      }

      if ( vlen )
      {
         *vlen = he->vlen;
      }

      return TTS_SUCCESS;

    }
    he = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,he->next);
  }

  return TTS_NOTFOUND;
}

int HashExCount(THashEx *ht)
{
  return ht->array->count;
}

THasheExIndex *HashExNext(THasheExIndex *hi)
{
  THasheExEntry *entry;

  hi->this = hi->next;
  
  if ( hi->this == 0 )
  {
    for ( ; hi->index < hi->ht->array->max+1; )
    {
      hi->this = hi->ht->array->array[hi->index++];
      if ( hi->this == 0 )
      {
        continue;
      }
      else
      {
        entry = (THasheExEntry *)ShmemPoolGetAddr(hi->ht->spool,hi->this);
        if ( entry->key == 0 )
        {
          continue;
        }
        break;
      }
    }
  }
  
  entry = (THasheExEntry *)ShmemPoolGetAddr(hi->ht->spool,hi->this);
  if ( entry == NULL || entry->key == 0 )
  {
    return NULL;
  }
  
  hi->next = entry->next;

  return hi;
}

THasheExIndex *HashExFirst(THashEx *ht)
{
  THasheExIndex *hi;

  if ( ht->array->count == 0 )
  {
    return NULL;
  }

  hi = &ht->iterator;

  hi->ht = ht;
  hi->index = 0;
  hi->next = 0;
  hi->this = 0;

  return HashExNext(hi);
}

int HashExThis(THasheExIndex *hi,char **key,int *klen,void **val,int *vlen)
{
  THasheExEntry *entry = (THasheExEntry *)ShmemPoolGetAddr(hi->ht->spool,hi->this);
  if ( entry == NULL )
  {
    return TTS_ENULL;
  }

  if ( key )
  {
    *key = (char *)ShmemPoolGetAddr(hi->ht->spool,entry->key);
  }

  if ( klen )
  {
    *klen = entry->klen;
  }

  if ( val )
  {
    *val = (void *)ShmemPoolGetAddr(hi->ht->spool,entry->val);
  }

  if ( vlen )
  {
    *vlen = entry->vlen;
  }

  return TTS_SUCCESS;
}

int HashExStorage(TMemPool *pool,THashEx *ht,THashExStore **store,int *bad)
{
  int             total,sum,i,j;
  THasheExEntry  *entry,*he;
  THashExStore   *tlt;

  if ( ht == NULL )
  {
    return TTS_EINVAL;
  }

  tlt = (THashExStore *)MemPoolAllocEx(pool,sizeof(THashExStore) * (ht->array->max) );
  total = sum = 0;

  for ( i = 0 ; i < ht->array->max ; i++ )
  {
    entry = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,ht->array->array[i]);
    for (; entry && entry->key != 0 ; )
    {
      sum++;
      entry = (THasheExEntry *)ShmemPoolGetAddr(ht->spool,entry->next);
    }

    for ( j = 0 ; sum && j < total + 1; j++)
    {
      if ( tlt[j].bad == sum )
      {
        tlt[j].total += sum;
        sum = 0;
        break;
      }
    }

    if ( sum )
    {
      tlt[total].bad = sum;
      tlt[total].total = sum;
      total++;
      sum = 0;
    }
  }
  
  *bad = total;
  *store = tlt;

  return TTS_SUCCESS;
}
