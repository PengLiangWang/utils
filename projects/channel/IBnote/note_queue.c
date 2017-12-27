#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "filename.h"
#include "logger.h"
#include "note_queue.h"
#include "note_error.h"

static int cmdQ = -1,ansQ = -1;
static int cmdFD = -1;

#define CMDLEN (sizeof(Cmd) - sizeof(long))

extern int errno;

#define HERE

#ifndef ENV_NOTECTRLFILE
#define ENV_NOTECTRLFILE "NOTECTRLFILE"
#endif

int note_queue_init(int server)
{
  char   *filename;
  char   *p;
  key_t  id;
  int    rv;

  if(cmdQ >= 0)
  {
    return 0;
  }

  p = getenv(ENV_NOTECTRLFILE);
  if(p == NULL)
  {
    logger(__FILE__,__LINE__,"note_queue_init() => 环境变量 %s 没有设置.",ENV_NOTECTRLFILE);
    return NOTE_FILEERROR;
  }

  filename = GetEtcFileName(p);
  if((id = ftok(filename,1)) == -1)
  {
    logger(__FILE__,__LINE__,"note_queue_init() => ftok(%s) errno %d,%s",filename,
                              errno,strerror(errno));
    return NOTE_FILEERROR;
  }

  if(server)
  {
    cmdFD = open(filename,O_RDWR);
    if(cmdFD < 0)
    {
      logger(__FILE__,__LINE__,"note_queue_init() => open(%s) errno %d,%s",filename,
                                errno,strerror(errno));
      return NOTE_FILEERROR;
    }

    rv = lockf(cmdFD,F_TLOCK,0);
    if(rv)
    {
      logger(__FILE__,__LINE__,"note_queue_init() => lockf(%s) errno %d,%s",filename,
                               errno,strerror(errno));
      close(cmdFD);
      return NOTE_FILEERROR;
    }
  }

  cmdQ = msgget(id,0666|(server?IPC_CREAT:0));
  if(cmdQ < 0)
  {
    logger(__FILE__,__LINE__,"note_queue_init() => msgget(CMDQ) errno %d,%s",
                              errno,strerror(errno));
    if(server)
    {
      close(cmdFD);
    }
    return NOTE_ERROR;
  }

  if((id = ftok(filename,2)) == -1)
  {
    logger(__FILE__,__LINE__,"note_queue_init() => ftok(%s) errno %d,%s",filename,
                              errno,strerror(errno));
    goto E;
  }

  ansQ = msgget(id,0666|(server?IPC_CREAT:0));
  if(ansQ < 0)
  {
    logger(__FILE__,__LINE__,"note_queue_init() => msgget(ANSQ) errno %d,%s",
                              errno,strerror(errno));
    goto E;
  }
HERE
  return 0;
E:
  if(server)
  {
    msgctl(cmdQ,IPC_RMID,NULL);  
    close(cmdFD);
  }
  cmdQ = -1; 
  return -1;
}

int note_queue_remove()
{
  if(cmdQ > 0)
  {
    msgctl(cmdQ,IPC_RMID,NULL);
    cmdQ = -1;
  }

  if(ansQ > 0)
  {
    msgctl(ansQ,IPC_RMID,NULL);
    ansQ = -1;
  }

  if(cmdFD >= 0)
  {
    close(cmdFD);
    cmdFD = -1;
  }

  return -1;
}

static int note_queue_send_cmd(Cmd *cmd)
{
  int  rv;

  if(cmdQ < 0)
  {
    rv = note_queue_init(0); 
    if(rv)
    {
      logger(__FILE__,__LINE__,"note_queue_send_cmd()=>note_queue_init() error %d",rv);
      return rv;
    }
  }

  cmd->mtype = (long)getpid();
  
  rv = msgsnd(cmdQ,(struct msgbuf *)cmd,CMDLEN,IPC_NOWAIT);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_send_cmd()=>msgsnd() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }

  return 0;
}

static int note_queue_recv_ans(Cmd *cmd)
{
  pid_t id;

  id = getpid();

  if(msgrcv(ansQ,(struct msgbuf *)cmd,CMDLEN,id,0) < 0)
  {
    logger(__FILE__,__LINE__,"note_queue_send_cmd()=>msgrcv() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }

  return 0;
}

int note_queue_cmd(Cmd *cmd)
{
  int  rv ;

HERE
  rv = note_queue_send_cmd(cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_cmd() => note_queue_send_cmd() error %d",rv);
    return rv;
  }
HERE
  rv = note_queue_recv_ans(cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_cmd() => note_queue_recv_ans() error %d",rv);
    return rv;
  }

HERE
  return cmd->ans;
}

int note_queue_recv_cmd(Cmd *cmd)
{
HERE
  if ( msgrcv(cmdQ,(struct msgbuf *)cmd,CMDLEN,0,0) < 0)
  {
    logger(__FILE__,__LINE__,"note_queue_recv_cmd()=>msgrcv() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }
HERE
  return 0;
}

int note_queue_send_ans(Cmd *ans)
{
  int rv;
HERE
  rv = msgsnd(ansQ,(struct msgbuf *)ans,CMDLEN,0);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_send_ans()=>msgsnd() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }
HERE
  return 0;
}

int note_queue_get(char *index,char *value)
{
  Cmd cmd;
  int rv;

  memset(&cmd,0,sizeof(cmd));
  cmd.cmd = NOTE_GET;
  strncpy(cmd.index_str,index,NOTEBUFSIZE);
  rv = note_queue_cmd(&cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_get() => note_queue_cmd() error %d",rv);
    return rv;
  }

  strncpy(value,cmd.value_str,NOTEBUFSIZE);
  return 0;
}

int note_queue_put(char *index,char *value)
{
  Cmd cmd;
  int rv;

  memset(&cmd,0,sizeof(cmd));
  cmd.cmd = NOTE_PUT;
  strncpy(cmd.index_str,index,NOTEBUFSIZE);
  strncpy(cmd.value_str,value,NOTEBUFSIZE);
  
  rv = note_queue_cmd(&cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_put() => note_queue_cmd() error %d",rv);
    return rv;
  }

  return 0;
}

int note_queue_reload()
{
  Cmd  cmd;
  int  rv ;

  memset(&cmd,0,sizeof(cmd));
  cmd.cmd = NOTE_RELOAD;
  rv =  note_queue_cmd(&cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_reload() => note_queue_cmd() error %d",rv);
    return rv;
  }

  return 0;
}

int note_queue_exit()
{
  Cmd  cmd;
  int  rv;

  memset(&cmd,0,sizeof(cmd));
  cmd.cmd = NOTE_EXIT;
  rv = note_queue_cmd(&cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_queue_exit() => note_queue_cmd() error %d",rv);
    return rv;
  }
  return 0;
}
