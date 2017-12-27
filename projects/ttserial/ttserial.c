#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ttserial.h"
#include "msgqueue.h"
#include "ttsys.h"

typedef struct
{
   long   mtype;
   char   name[33];
   long   logNo;
   int    batchNo;
}SerialControl;

static TMessageQueue   QueueQ,QueueA;

int GetSerialNo(int serverId,char *name,char *logNo,int len)
{
   static int            selfPid ;
   static int            initFlag = 0;
   static int            res ;
   static SerialControl  QScm,AScm;

   selfPid = (int)getpid();

   if ( initFlag == 0 )
   {
      uint32    QId,AId;
      char      path[256];

      snprintf(path,256,"%s/etc",(char *)getenv("HOME"));
      QId = ftok(path,serverId);
      AId = ftok(path,serverId + 1);
      if ( QId < 0 || AId < 0 )
      {
         ELOG(ERROR,"�ļ�ϵͳ(ftok)���ֹ���(%s:%d)", GetStrError(errno), errno);
         return errno;
      }
      res = MessageQueueOpen(&QueueQ,QId);
      if ( res )
      {
         ELOG(ERROR,"������ˮ������(%d)ʧ��,������Ϣ(%s:%d)",serverId,GetStrError(res),res);
         return res;
      }

      res = MessageQueueOpen(&QueueA,AId);
      if ( res )
      {
         ELOG(ERROR,"������ˮ������(%d)ʧ��,������Ϣ(%s:%d)",serverId,GetStrError(res),res);
         return res;
      }

      initFlag = 1;
   }
   
   memset(&QScm,0,sizeof(SerialControl));
   memset(&AScm,0,sizeof(SerialControl));
   
   QScm.mtype = selfPid;
   strncpy(QScm.name,name,32);
   QScm.logNo = -1;
   QScm.batchNo = -1;

   res = MessageQueueSend(&QueueQ,(TMessageQueueBuffer *)&QScm,sizeof(QScm) - sizeof(long));
   if ( res )
   {
      ELOG(ERROR,"������ˮ������(%d)ʧ��,������Ϣ(%s:%d)",serverId,GetStrError(res),res);
      if ( res == EIDRM )
      {
         initFlag = 0;
      }
      return res;
   }

   AScm.mtype = selfPid;
  
   res = MessageQueueRecv(&QueueA,(TMessageQueueBuffer *)&AScm,sizeof(AScm) - sizeof(long));
   if ( res )
   {
      ELOG(ERROR,"������ˮ����(%d)Ӧ��ʧ��,������Ϣ(%s:%d)",serverId,GetStrError(res),res);
      if ( res == EIDRM )
      {
         initFlag = 0;
      }
      return res;
   }

   if ( logNo && len > 0 )
   {
      sprintf(logNo,"%0*d",len,AScm.logNo);
   }
   
   if ( AScm.logNo < 0 )
   {
      ELOG(ERROR,"��ˮ������(%d)�������ˮ�ŷǷ�(TTS_EINVAL)",serverId);
      return TTS_EINVAL;
   }

   return 0;
}

