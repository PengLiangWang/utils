#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>

#include "mutex.h"


union semun 
{
  int              val;      /* value for SETVAL */
  struct semid_ds *buf;      /* buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;    /* array for GETALL, SETALL */
                             /* Linux specific part: */
  struct seminfo  *__buf;    /* buffer for IPC_INFO */
};

static struct sembuf mutex_op_on;
static struct sembuf mutex_op_off;

static void MutexSetup(int idx)
{
  mutex_op_on.sem_num = idx - 1;
  mutex_op_on.sem_op = -1;
  mutex_op_on.sem_flg = SEM_UNDO;

  mutex_op_off.sem_num = idx - 1;
  mutex_op_off.sem_op = 1;
  mutex_op_off.sem_flg = SEM_UNDO | IPC_NOWAIT;
}

static int MutexCleanup(TMutex *mutex)
{
  union semun ick;

  if(mutex->semid != -1)
  {
    ick.val = 0;
    semctl(mutex->semid,0,IPC_RMID,ick);
    mutex->semid = -1;
  }

  return TTS_SUCCESS;
  
}

int MutexCreate(TMutex *mutex,int key,int nsems,const char *name)
{
  union semun   ick;
  int      rv;
  int           i;

  memset(mutex,0,sizeof(TMutex));
  mutex->semkey = key;
  mutex->semid = -1;
  mutex->nsems = nsems;

  mutex->semid = semget(key, nsems, IPC_CREAT | 0600);
  if (mutex->semid < 0)
  {
    rv = errno;
    MutexCleanup(mutex);
    return rv;
  }

  ick.val = 1;

  for (i = 0; i < nsems ; i++)
  {
    if (semctl(mutex->semid, i, SETVAL , ick) < 0)
    {
      rv = errno;
      MutexCleanup(mutex);
      return rv;
    }
  }

  mutex->curr_locked = 0;
  strncpy(mutex->name,name,MUTEX_NAME_MAX_LEN);

  return TTS_SUCCESS;
}

int MutexOpen(TMutex *mutex,int key,int nsems,const char *name)
{
  union semun   ick;
  int           rv;
  int           i;

  memset(mutex,0,sizeof(TMutex));
  mutex->semkey = key;
  mutex->semid = -1;
  mutex->nsems = nsems;

  mutex->semid = semget(key, nsems, 0);
  if (mutex->semid < 0)
  {
    return errno;
  }

  mutex->curr_locked = 0;
  strncpy(mutex->name,name,MUTEX_NAME_MAX_LEN);

  return TTS_SUCCESS;
}

int MutexAcquire(TMutex *mutex,int nsem)
{
  int  rc;

  MutexSetup(nsem); 
  rc = semop(mutex->semid, &mutex_op_on, 1);
  if (rc < 0) 
  {
    return errno;
  }

  mutex->curr_locked = 1;
  
  return TTS_SUCCESS;
}

int MutexRelease(TMutex *mutex,int nsem)
{
  int rc;

  MutexSetup(nsem);

  mutex->curr_locked = 0;
  
  rc = semop(mutex->semid, &mutex_op_off, 1);
  if (rc < 0) 
  {
    return errno;
  }
  
  return TTS_SUCCESS;
}

int MutexDestory(TMutex *mutex)
{
  MutexCleanup(mutex);

  return TTS_SUCCESS;
}

