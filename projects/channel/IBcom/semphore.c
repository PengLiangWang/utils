#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#define PERM 0660
#define DUMMY 0
#define COUNT 4
#define SEMA_INDEX  0
#define SEMA_NUMBERS  1
#define SEMA_INIT_VALUE  1
#define INCREASE  1
#define DECREASE   -1
enum BOOLEAN {MY_FALSE,MY_TRUE} creator=MY_FALSE;

  static int semid,pid,i,semnum=1;
  static struct sembuf lock={SEMA_INDEX,DECREASE,SEM_UNDO};
  static struct sembuf unlock={SEMA_INDEX,INCREASE,SEM_UNDO|IPC_NOWAIT};
static int semaphoreGetKey(int *key,int server)
{
  char    *filename;
  int     r;
  int     fd;

  filename=(char *)GetEtcFileName("IBSEMAPHORE");
  if ((*key = ftok(filename,1)) == -1)
    return -1;
  if (server)
  {
    fd=open(filename,O_RDWR);
    if (fd<0)
      return -1;
    r = lockf(fd,F_TLOCK,0);
    if (r!=0)
    {
      perror("LOCK SEMAPHORE FILE ERROR");
      close(fd);
      return -1;
    }
  }
  return 0;
}

int semaphoreInit(int server)
{
  int  r,keyval;

  r=semaphoreGetKey(&keyval,server);
  if(r) 
    return r;

  if((semid=semget(keyval,semnum,IPC_CREAT|IPC_EXCL|PERM))!=-1)
    creator=MY_TRUE;
  else
    if((semid=semget(keyval,semnum,DUMMY))==-1)
    {
      return -1;
    }
  if(creator)
  {
    if(semctl(semid,SEMA_INDEX,SETVAL,SEMA_INIT_VALUE)==-1)
    {
      return -1;
    }
  }
  return 0;
}


int semaphoreLock()
{
  if(semop(semid,&lock,SEMA_NUMBERS)==-1)
  {
    return -1;
  }
  return 0;  
}

int semaphoreUnlock()
{
  if(semop(semid,&unlock,SEMA_NUMBERS)==-1)
  {
    return -1;
  }
  return 0;  
}

int semaphoreDel()  
{
  if(creator)
  {
    if(semctl(semid,SEMA_INDEX,IPC_RMID,DUMMY)==-1)
    {
      return -1;  
    }
  }
}

