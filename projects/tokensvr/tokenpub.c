#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ttsys.h"

static int   InitFlag = 0;

static TMessageQueue QueueQ,QueueA;

int  TokenQueueOpen(int id1 , TMessageQueue **queueQ,  TMessageQueue **queueA )
{
   if ( InitFlag )
   {
      *queueQ = &QueueQ;
      *queueA = &QueueA;
   }
   else
   {
      char   path[256];
      int    res;
      int    QId,AId;

      snprintf(path, 256, "%s/etc/.token",(char *)getenv("HOME"));
      
      if ( access(path, 0) != 0)
      {
         if ( mkdir(path, 0755) != 0 )
         {
            ELOG(ERROR, "token server director can't created!");
            return errno;
         }
      }

      QId = ftok(path, id1);
      AId = ftok(path, id1 + 1);

      if ( QId < 0 || AId < 0 )
      {
         ELOG(ERROR, "token server function ftok error (%s:%d)", GetStrError(errno), errno);
         return errno;
      }

      res = MessageQueueOpen(&QueueQ,QId);
      if ( res )
      {
         ELOG(ERROR, " can't open token server queue Q! (%s:%d)", GetStrError(res), res);
         return res;
      }

      res = MessageQueueOpen(&QueueA,AId);
      if ( res )
      {
         ELOG(ERROR, " can't open token server queue A! (%s:%d)", GetStrError(res), res);
         return res;
      }

      InitFlag = 1;

      *queueQ = &QueueQ;
      *queueA = &QueueA;
   }

   return 0;
}

int ToketQueueClose(int id1)
{
   if ( InitFlag )
   {
      MessageQueueDestroyByID(QueueQ.qid);
      MessageQueueDestroyByID(QueueA.qid);
      return 0;
   }
   else
   {
      char   path[256];
      int    res;
      int    QId,AId;

      snprintf(path, 256, "%s/etc/.token",(char *)getenv("HOME"));
      
      if ( access(path, 0) != 0)
      {
         if ( mkdir(path, 0755) != 0 )
         {
            ELOG(ERROR, "token server director can't created!");
            return errno;
         }
      }

      QId = ftok(path, id1);
      AId = ftok(path, id1 + 1);

      if ( QId > 0 )
      {
         MessageQueueDestory(QId);
      }
      if ( AId > 0 )
      {
         MessageQueueDestory(AId);
      }

      return 0;
   }
   

}

int TokenQueueCreate(int id1, TMessageQueue **queueQ, TMessageQueue **queueA )
{
   if ( InitFlag )
   {
      *queueQ = &QueueQ;
      *queueA = &QueueA;
   }
   else
   {
      char   path[256];
      int    res;
      int    QId,AId;

      snprintf(path, 256, "%s/etc/.token",(char *)getenv("HOME"));
      
      if ( access(path, 0) != 0)
      {
         if ( mkdir(path, 0755) != 0 )
         {
            ELOG(ERROR, "token server director can't created!");
            return errno;
         }
      }

      QId = ftok(path, id1);
      AId = ftok(path, id1 + 1);

      if ( QId < 0 || AId < 0 )
      {
         ELOG(ERROR, "token server function ftok error (%s:%d)", GetStrError(errno), errno);
         return errno;
      }

      res = MessageQueueCreate(&QueueQ,QId);
      if ( res )
      {
         ELOG(ERROR, " can't create token server queue Q! (%s:%d)", GetStrError(res), res);
         return res;
      }

      res = MessageQueueCreate(&QueueA,AId);
      if ( res )
      {
         ELOG(ERROR, " can't create token server queue A! (%s:%d)", GetStrError(res), res);
         MessageQueueDestory(QId);
         return res;
      }

      InitFlag = 1;
      *queueQ = &QueueQ;
      *queueA = &QueueA;
   }

   return 0;
}
