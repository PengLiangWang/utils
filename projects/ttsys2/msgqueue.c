
#include <sys/types.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "msgqueue.h"


static int GetMessageQueueKeyByName(TMessageQueue *MessageQueue)
{
  char  name[256 + 1];
  int    fd = 0;

  snprintf(name,256,"%s/etc/.%s",getenv("HOME"),MessageQueue->name);
  

  fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if(fd < 0)
  {
    return errno;
  }

  close(fd);
  
  MessageQueue->key = ftok(name,1);

  if (MessageQueue->key < 0)
  {
    return errno;
  }

  MessageQueue->flag = 1;

  return TTS_SUCCESS;
}

static int DeleteMessageQueueName(TMessageQueue *MessageQueue)
{
  char  name[256 + 1];

  snprintf(name,256,"%s/etc/.%s",getenv("HOME"),MessageQueue->name);

  unlink(name);
  
  return TTS_SUCCESS;

}

int MessageQueueCreate(TMessageQueue *MessageQueue,uint32 key)
{
  TMessageQueue    *new;
  int              rv;
 

  new = MessageQueue;
  
  memset(new,0,sizeof(TMessageQueue));

  new->key  = (uint32)key;

  if ( ( new->qid = msgget(new->key, 0660 | IPC_CREAT) ) < 0)
  {
    return errno;
  }
    
  return TTS_SUCCESS;

}

int MessageQueueCreateEx(TMessageQueue *MessageQueue,const char *name)
{
  TMessageQueue    *new;
  int          rv;
 
  new = MessageQueue;
  
  memset(new, 0, sizeof(TMessageQueue));

  strncpy(new->name,name,MESSAGE_QUEUE_NAME_MAX_LEN);
  
  if ( ( rv = GetMessageQueueKeyByName(new) ) != TTS_SUCCESS )
  {
    return rv;
  }

  if ( ( new->qid = msgget(new->key, 0660 | IPC_CREAT) ) < 0 )
  {
    return errno;
  }
    
  return TTS_SUCCESS;

}

int MessageQueueDestroyByID(uint32 qid)
{
  if ( qid < 0 )
  {
    return 0;
  }

  if (msgctl(qid, IPC_RMID, NULL) == -1)
  {
    return errno;
  }

  return TTS_SUCCESS;
}

int MessageQueueDestory(uint32 key)
{
  uint32              qid;
  
  if ( key <= 0 )
  {
    return 0;
  }

  if ((qid = msgget((key_t)key, 0)) < 0)
  {
    return errno;
  }

  if (msgctl(qid, IPC_RMID, NULL) == -1)
  {
    return errno;
  }

  return TTS_SUCCESS;

}

int MessageQueueDestoryEx(const char *name)
{
  int              qid;
  TMessageQueue    new;
  int         rv;

  memset(&new,0,sizeof(TMessageQueue));

  strncpy(new.name,name,MESSAGE_QUEUE_NAME_MAX_LEN);

  if ( ( rv = GetMessageQueueKeyByName(&new) ) != TTS_SUCCESS )
  {
    return rv;
  }

  if ( ( qid = msgget(new.key, 0) ) < 0 )
  {
    return errno;
  }

  DeleteMessageQueueName(&new);

  if (msgctl(qid, IPC_RMID, NULL) == -1)
  {
    return errno;
  }

  return TTS_SUCCESS;

}

int  MessageQueueOpen(TMessageQueue *MessageQueue,uint32 key)
{
  TMessageQueue   *new;
  int         rv;

  new = MessageQueue;
  
  memset(new,0,sizeof(TMessageQueue));

  new->key  = (uint32)key;

  if ((new->qid = msgget(new->key, 0)) < 0)
  {
    return errno;
  }
    
  return TTS_SUCCESS;

}

int  MessageQueueOpenByID(TMessageQueue *MessageQueue,uint32 qid)
{
  TMessageQueue   *new;
  int              rv;
  struct msqid_ds  msgds;


  if ( qid < 0 )
  {
    return TTS_EINVAL;
  }

  new = MessageQueue;
  
  memset(new,0,sizeof(TMessageQueue));

  memset(&msgds, 0, sizeof(struct msqid_ds));
  if ( msgctl(qid, IPC_STAT, &msgds) == -1)
  {
    return errno;
  }

  new->qid = qid;

  return TTS_SUCCESS;

}

int  MessageQueueOpenEx(TMessageQueue *MessageQueue,const char *name)
{
  TMessageQueue   *new;
  int         rv;

  new = MessageQueue;
  
  memset(new,0,sizeof(TMessageQueue));

  strncpy(new->name,name,MESSAGE_QUEUE_NAME_MAX_LEN);
  
  if ( ( rv = GetMessageQueueKeyByName(new) ) != TTS_SUCCESS )
  {
    return rv;
  }

  if ((new->qid = msgget(new->key, 0)) < 0)
  {
    return errno;
  }
    
  return TTS_SUCCESS;

}

int MessageQueueSend(TMessageQueue *MessageQueue,TMessageQueueBuffer *buffer,uint32 size)
{
  if (MessageQueue->qid < 0)
  {
    return  TTS_EINVAL;
  }
  
  if (msgsnd(MessageQueue->qid,(struct msgbuf *)buffer,size,0))
  {
    return errno;
  }

  return TTS_SUCCESS;
}

int MessageQueueRecv(TMessageQueue *MessageQueue,TMessageQueueBuffer *buffer,uint32 size)
{
  if (MessageQueue->qid < 0)
  {
    return  TTS_EINVAL;
  }

  if ( msgrcv(MessageQueue->qid,(struct msgbuf *)buffer,size,buffer->mtype,0) < 0 )
  {
    return errno;
  }

  return TTS_SUCCESS;
}

int MessageQueueTimeRecv(TMessageQueue *MessageQueue,TMessageQueueBuffer *buffer,uint32 size,int times)
{
  int         rv;
  extern int  errno;
  time_t      start,now;

  
  if ( MessageQueue->qid < 0 )
  {
    return  TTS_EINVAL;
  }
  
  errno = 0;

  time(&start);
  
  for (; ; )
  {
     time(&now);
     if ( ( now - start ) > times )
     {
        return TTS_TIMEOUT;
     }
     rv = msgrcv(MessageQueue->qid,(struct msgbuf *)buffer,size,buffer->mtype, IPC_NOWAIT );
     if ( rv == -1 )
     {
        if ( errno == EINTR )
        {
           return errno;
        }
        else if ( errno == ENOMSG )
        {
           ;
        }
        else
        {
           return errno;
        }
     }
     else if ( rv == size )
     {
        break;
     }
     usleep(1000 * 200);
  }

  return TTS_SUCCESS;
}

static int timeouted = 0;

static void MessageQueueTimeout(int signo)
{
  timeouted = 1;
  signal(signo, SIG_IGN);
  return ;
}


int MessageQueueRecvEx(TMessageQueue *MessageQueue,TMessageQueueBuffer *buffer,uint32 size,int times)
{
  int    rv;
  extern int  errno;
  
  if ( MessageQueue->qid < 0 )
  {
    return  TTS_EINVAL;
  }
  
  timeouted = 0;

  errno = 0;

  alarm(times);
  signal(SIGALRM, MessageQueueTimeout);
  if ( msgrcv(MessageQueue->qid,(struct msgbuf *)buffer,size,buffer->mtype,0) < 0 )
  {
    signal(SIGALRM, SIG_IGN);
    alarm(0);
    if (errno == EINTR && timeouted)
    {
      return TTS_TIMEOUT;
    }
    return errno;
  }
  signal(SIGALRM, SIG_IGN);
  alarm(0);
  
  return TTS_SUCCESS;
}

