#include <errno.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include "shmem.h"

int ShmemCreate(TShmem *new,uint32 size,int key,ulong32 shmbase)
{
  memset(new,0,sizeof(TShmem));

  new->key = (uint32)key;

  new->size = size;

  if ( ( new->shmid = shmget(new->key, size, IPC_EXCL | IPC_CREAT | SHM_R | SHM_W ) ) < 0 )
  {
    return errno;
  }
  
  if ( shmbase <= 0 )
  {
    if ( ( new->base = shmat(new->shmid, NULL, 0 ) ) == (void *)-1 )
    {
      return errno;
    }
  }
  else
  {
    if ((new->base = shmat(new->shmid, (const void *)shmbase, 0)) == (void *)-1)
    {
      return errno;
    }
  }
    
  new->usable = new->base;
  
  return TTS_SUCCESS;
}

int ShmemAttach(TShmem *new,uint32 size,int key,ulong32 shmbase)
{
  memset(new,0,sizeof(TShmem));


  new->key = (uint32)key;
  new->size = size;

  if ((new->shmid = shmget(new->key, new->size,SHM_R | SHM_W)) < 0) 
  {
    return errno;
  }

  if ( shmbase <= 0 )
  {
    if ((new->base = shmat(new->shmid, NULL, 0)) == (void *)-1)
    {
      return errno;
    }
  }
  else
  {
    if ((new->base = shmat(new->shmid, (const void *)shmbase, 0)) == (void *)-1)
    {
      return errno;
    }
  }
    
  new->usable = new->base;
  
  return TTS_SUCCESS;

}

int ShmemDetach(TShmem *m)
{
  if (shmdt(m->base) == -1)
  {
    return errno;
  }
  
  m->base = (void *)0;
  
  return TTS_SUCCESS;
}

int ShmemDestory(TShmem *m)
{
    
  if (shmctl(m->shmid, IPC_RMID, NULL) == -1)
  {
    return errno;
  }

  if (shmdt(m->base) == -1) 
  {
    return errno;
  }

  m->base = (void *)0;

  return TTS_SUCCESS;
}

int ShmemGetNattch(TShmem *m)
{
  struct shmid_ds  buf;
   
  memset(&buf, 0, sizeof(buf));
  if (shmctl(m->shmid, IPC_STAT, &buf) == -1)
  {
    return errno;
  }

  m->nattch = (int)buf.shm_nattch;

  return TTS_SUCCESS;
}

void *ShmemGetBaseAddress(const TShmem *m)
{
  return m->usable;
}


uint32 ShmemGetSize(const TShmem *m)
{
  return m->size;
}

