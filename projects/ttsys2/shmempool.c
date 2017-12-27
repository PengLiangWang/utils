#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

#include "shmempool.h"

struct stru_shmem_alloctor_t
{
  int     free;      /* 可分配的共享内存的大小 */
  int     used;      /* 已使用的共享内存大小 */
  int     offset;    /* 当前的偏移量 */
};

typedef struct stru_shmem_alloctor_t TShmemAlloctor;

struct stru_shmem_pool_t
{
  TMemPool         *pool;                 /* 内存池 */
  TShmem           *shmem;                /* 共享内存 */
  int               proj;                 /* 项目ID<唯一>*/
  int               offset;               /* 共享内存池中Alloctor起始地址 */
  TShmemAlloctor   *alloctor;             /* 引擎内存分配器 */
  long              shmaddr;              /* 共享内存基地址 */
  long              baseaddr;             /* 整个内存基地址 */
};

static int ShmemPoolCleanup(void *the)
{
  TShmemPool  *spool = (TShmemPool *)the;
  
  if ( spool->shmem != NULL )
  {
    ShmemDetach(spool->shmem);
  }

  spool->shmem = NULL;

  return TTS_SUCCESS;
}

int ShmemPoolDestory(TShmemPool *spool)
{
  if ( spool->shmem != NULL )
  {
    ShmemDestory(spool->shmem);
  }
  spool->shmem = NULL;

  return TTS_SUCCESS;
}

char *ShmemPoolGetBaseAddr(TShmemPool *spool,int size)
{
  if ( spool && spool->offset == size )
  {
    return (char *)spool->baseaddr;
  }
  return NULL;
}

int ShmemPoolCreate(TMemPool *pool,int proj,int size,int offset,TShmemPool **spool)
{
  TShmemPool *shmpool;
  TShmem     *shmem;
  char       *home;
  int         rv;
  int         key;

  shmpool = (TShmemPool *)MemPoolAllocEx(pool,sizeof(TShmemPool));
  if ( shmpool == NULL )
  {
    return TTS_ENOMEM;
  }

  shmpool->pool = pool;
  shmpool->proj = proj;
  

  shmem = (TShmem *)MemPoolAllocEx(pool,sizeof(TShmem));
  if ( shmem == NULL )
  {
    return TTS_ENOMEM;
  }

  home = (char *)getenv("HOME");
  if ( home == NULL )
  {
    return TTS_EACCES;
  }

  key = ftok((const char *)home,proj);
  if ( key < 0 )
  {
    return errno;
  }

  rv = ShmemCreate(shmem,(uint32)size,key,0);
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }

  shmpool->shmaddr = shmpool->baseaddr = (long)ShmemGetBaseAddress(shmem);
  shmpool->shmem = shmem;

  memset((char *)shmpool->baseaddr,0,size);
  
  if ( offset > 0 )
  {
    shmpool->shmaddr += offset;
    shmpool->offset = offset;
  }
  
  shmpool->alloctor = (TShmemAlloctor *)shmpool->shmaddr;
  shmpool->alloctor->free = size - offset - sizeof(TShmemAlloctor);
  shmpool->alloctor->used = 0;
  shmpool->alloctor->offset = 1;
  
  shmpool->shmaddr += sizeof(TShmemAlloctor);

  MemPoolRegisterCleanup(pool,shmpool,ShmemPoolCleanup);

  *spool = shmpool;

  return TTS_SUCCESS;
}


int ShmemPoolCheck(int proj,int size)
{
  TShmem      shmem;
  char       *home;
  int         rv;
  int         key;


  home = (char *)getenv("HOME");
  if ( home == NULL )
  {
    return TTS_EACCES;
  }

  key = ftok((const char *)home,proj);
  if ( key < 0 )
  {
    return errno;
  }
  
  memset(&shmem, 0, sizeof(shmem));
  
  rv = ShmemAttach(&shmem,(uint32)size,key,0);
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }
  
  ShmemDetach(&shmem);
  
  return TTS_SUCCESS;
}

int ShmemPoolGetShmem(TShmemPool *spool, TShmem *shmem)
{
   if ( !shmem || !spool )
   {
      return TTS_ENOMEM;
   }

   memset(shmem, 0, sizeof(TShmem));
   
   memcpy(shmem, spool->shmem, sizeof(TShmem));

   return TTS_SUCCESS;
}

int ShmemPoolOpenEx(TMemPool *pool,int proj,int size,int offset,TShmemPool **spool)
{
  TShmemPool *shmpool;
  TShmem     *shmem;
  char       *home;
  int         rv;
  int         key;

  shmpool = (TShmemPool *)MemPoolAllocEx(pool,sizeof(TShmemPool));
  if ( shmpool == NULL )
  {
    return TTS_ENOMEM;
  }

  shmpool->pool = pool;
  shmpool->proj = proj;
  

  shmem = (TShmem *)MemPoolAllocEx(pool,sizeof(TShmem));
  if ( shmem == NULL )
  {
    return TTS_ENOMEM;
  }

  home = (char *)getenv("HOME");
  if ( home == NULL )
  {
    return TTS_EACCES;
  }

  key = ftok((const char *)home,proj);
  if ( key < 0 )
  {
    return errno;
  }

  rv = ShmemAttach(shmem,(uint32)size,key,0);
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }

  shmpool->shmaddr = shmpool->baseaddr = (long)ShmemGetBaseAddress(shmem);
  shmpool->shmem = shmem;

  memset((char *)shmpool->baseaddr,0,size);
  
  if ( offset > 0 )
  {
    shmpool->shmaddr += offset;
    shmpool->offset = offset;
  }
  
  shmpool->alloctor = (TShmemAlloctor *)shmpool->shmaddr;
  shmpool->alloctor->free = size - offset - sizeof(TShmemAlloctor);
  shmpool->alloctor->used = 0;
  shmpool->alloctor->offset = 1;
  
  shmpool->shmaddr += sizeof(TShmemAlloctor);

  MemPoolRegisterCleanup(pool,shmpool,ShmemPoolCleanup);

  *spool = shmpool;

  return TTS_SUCCESS;

}

int ShmemPoolOpen(TMemPool *pool,int proj,int size,int offset,TShmemPool **spool)
{
  TShmemPool *shmpool;
  TShmem     *shmem;
  char       *home;
  int    rv;
  int         key;

  shmpool = (TShmemPool *)MemPoolAllocEx(pool,sizeof(TShmemPool));
  if ( shmpool == NULL )
  {
    return TTS_ENOMEM;
  }

  shmpool->pool = pool;
  shmpool->proj = proj;
  

  shmem = (TShmem *)MemPoolAllocEx(pool,sizeof(TShmem));
  if ( shmem == NULL )
  {
    return TTS_ENOMEM;
  }

  home = (char *)getenv("HOME");
  if ( home == NULL )
  {
    return TTS_EACCES;
  }

  key = ftok((const char *)home,proj);
  if ( key < 0 )
  {
    return errno;
  }

  rv = ShmemAttach(shmem,(uint32)size,key,0);
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }

  shmpool->shmaddr = shmpool->baseaddr = (long)ShmemGetBaseAddress(shmem);
  shmpool->shmem = shmem;
  
  if ( offset > 0 )
  {
    shmpool->shmaddr += offset;
    shmpool->offset = offset;
  }
  
  shmpool->alloctor = (TShmemAlloctor *)shmpool->shmaddr;
  
  shmpool->shmaddr += sizeof(TShmemAlloctor);

  MemPoolRegisterCleanup(pool,shmpool,ShmemPoolCleanup);

  *spool = shmpool;

  return TTS_SUCCESS;
}

char *ShmemPoolAlloc(TShmemPool *spool,int size)
{
  char  *ptr;

  if ( spool->alloctor->used + size > spool->alloctor->free - 1)
  {
    return NULL;
  }

  ptr = (char *)(spool->shmaddr + spool->alloctor->offset);

  spool->alloctor->offset += size;
  spool->alloctor->used  += size;

  return ptr;
}

char *ShmemPoolAllocEx(TShmemPool *spool,int size,int *addr)
{
  char  *ptr;

  if ( spool->alloctor->used + size > spool->alloctor->free - 1)
  {
    return NULL;
  }

  ptr = (char *)(spool->shmaddr + spool->alloctor->offset);

  spool->alloctor->offset += size;
  spool->alloctor->used  += size;

  if ( addr )
  {
    *addr = (int)(ptr - spool->shmaddr);
  }

  return ptr;

}

char *ShmemPoolGetAddr(TShmemPool *spool,int offset)
{
  if ( offset <= 0 )
  {
    return NULL;
  }
  else
  {
    return (char *)(spool->shmaddr + offset);
  }
}

int ShmemGetOffset(TShmemPool *spool,char *data)
{
  if ( data == NULL )
  {
    return 0;
  }
  else
  {
    return (int)(data - spool->shmaddr);
  }
}

uint32 ShmemPoolGetUsed(TShmemPool *spool)
{
  if ( spool && spool->alloctor )
  {
    return spool->alloctor->used;
  }
  return -1;
}

uint32 ShmemPoolGetFree(TShmemPool *spool)
{
  if ( spool && spool->alloctor )
  {
    return spool->alloctor->free;
  }
  return -1;
}

uint32 ShmemPoolGetTotal(TShmemPool *spool)
{
  if ( spool &&  spool->shmem )
  {
     return ShmemGetSize(spool->shmem);
  }
}
