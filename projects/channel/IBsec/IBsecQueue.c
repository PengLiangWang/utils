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
#include "IBsecQueue.h"

static int cmdQ = -1,ansQ = -1;
static int cmdFD = -1;

#define CMDLEN (sizeof(IBCMDINFO) - sizeof(long))

extern int errno;

#define ENV_IBSECCTRLFILE      "IBSECCTRLFILE"    //IBSEC 控制文件

int sec_queue_init(int server)
{
  char   *filename;
  key_t  id;
  int    rv;

  if(cmdQ >= 0)
  {
    return 0;
  }

  filename = GetEtcFileName(getenv(ENV_IBSECCTRLFILE));
  if((id = ftok(filename,1)) == -1)
  {
    logger(__FILE__,__LINE__,"安全模块初始化失败,信息 ftok(%s) errno %d,%s",filename,
                              errno,strerror(errno));
    return -1;
  }

  cmdQ = msgget(id,0666|(server?IPC_CREAT:0));
  if(cmdQ < 0)
  {
    logger(__FILE__,__LINE__,"安全模块初始化失败,信息 msgget(CMDQ) errno %d,%s",
                              errno,strerror(errno));
    return -1;
  }

  if((id = ftok(filename,2)) == -1)
  {
    logger(__FILE__,__LINE__,"安全模块初始化失败,信息 ftok(%s) errno %d,%s",filename,
                              errno,strerror(errno));
    goto E;
  }

  ansQ = msgget(id,0666|(server?IPC_CREAT:0));
  if(ansQ < 0)
  {
    logger(__FILE__,__LINE__,"安全模块初始化失败,信息 msgget(ANSQ) errno %d,%s",
                              errno,strerror(errno));
    goto E;
  }

  return 0;
E:
  if(server)
  {
    msgctl(cmdQ,IPC_RMID,NULL);  
  }

  cmdQ = -1;

  return -1;
}

int sec_queue_lock()
{
  char   *filename;
  int     rv;

  filename = GetEtcFileName(getenv(ENV_IBSECCTRLFILE));

  cmdFD = open(filename,O_RDWR);
  if(cmdFD < 0)
  {
    logger(__FILE__,__LINE__,"安全模块初始化失败,信息 open(%s) errno %d,%s",filename,
                              errno,strerror(errno));
    return -1;
  }

  rv = lockf(cmdFD,F_TLOCK,0);
  if(rv)
  {
    logger(__FILE__,__LINE__,"安全模块初始化失败,信息 lockf(%s) errno %d,%s",filename,
                             errno,strerror(errno));
    close(cmdFD);
    return -1;
  }

  return 0;
}


int sec_queue_remove()
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

int sec_queue_send_cmd(IBCMDINFO *cmd)
{
  int  rv;

  if(cmdQ < 0)
  {
    rv = sec_queue_init(0);
    if(rv)
    {
      logger(__FILE__,__LINE__,"与安全模块通讯失败,信息 sec_queue_init() error %d",rv);
      return rv;
    }
  }

  cmd->mtype = (long)getpid();
  
  rv = msgsnd(cmdQ,(struct msgbuf *)cmd,CMDLEN,IPC_NOWAIT);
  if(rv)
  {
    logger(__FILE__,__LINE__,"与安全模块通讯失败,信息 msgsnd() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }

  return 0;
}

int sec_queue_recv_ans(IBCMDINFO *cmd)
{
  pid_t id;

  id = getpid();

  if(msgrcv(ansQ,(struct msgbuf *)cmd,CMDLEN,id,0) < 0)
  {
    logger(__FILE__,__LINE__,"与安全模块通讯失败,信息 msgrcv() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }

  return 0;
}

int sec_queue_recv_cmd(IBCMDINFO *cmd)
{
  if ( msgrcv(cmdQ,(struct msgbuf *)cmd,CMDLEN,0,0) < 0)
  {
    logger(__FILE__,__LINE__,"与安全队列通讯失败,信息 msgrcv() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }
  return 0;
}

int sec_queue_send_ans(IBCMDINFO *ans)
{
  int rv;

  rv = msgsnd(ansQ,(struct msgbuf *)ans,CMDLEN,0);
  if(rv)
  {
    logger(__FILE__,__LINE__,"与安全队列通讯失败,信息 msgsnd() errno %d,%s",errno,
                             strerror(errno));
    return -1;
  }
  return 0;
}

int sec_queue_cmd(IBCMDINFO *cmd)
{
  int  rv ;

  rv = sec_queue_send_cmd(cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"与安全模块通讯失败,信息 sec_queue_send_cmd() error %d",rv);
    return rv;
  }

  rv = sec_queue_recv_ans(cmd);
  if(rv)
  {
    logger(__FILE__,__LINE__,"与安全模块通讯失败,信息 sec_queue_recv_ans() error %d",rv);
    return rv;
  }

  return cmd->ans;
}

