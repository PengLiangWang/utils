#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "ttsys.h"
#include "etoken.h"


static TMessageQueue  *QueueQ = NULL , *QueueA = NULL;
static TTokenMsg     tokenMsg;

int TokenGet(int id, int *maxTrans, int *currTrans)
{
   int  res ;

   res = TokenQueueOpen(id, &QueueQ, &QueueA);
   if ( res )
   {
      ELOG(ERROR, "´ò¿ªÁîÅÆ¶ÓÁÐÊ§°Ü");
      return res;
   }

   memset(&tokenMsg, 0, sizeof(TTokenMsg));

   tokenMsg.mtype = (long)getpid();
   tokenMsg.rtype = ETOKEN_INCREASE_CURRENT;

   res = MessageQueueSend(QueueQ, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long));
   if ( res )
   {
      ELOG(ERROR, "·¢ËÍÁîÅÆÇëÇóÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   res = MessageQueueRecvEx(QueueA, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long), 5 );
   if ( res )
   {
      ELOG(ERROR, "½ÓÊÜÁîÅÆÓ¦´ðÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   if ( maxTrans )
   {
      *maxTrans = tokenMsg.tmax;
   }

   if ( currTrans )
   {
      *currTrans = tokenMsg.tnum;
   }

   if ( tokenMsg.tcode != ETOKEN_ERR_SUCCESS )
   {
      return tokenMsg.tcode;
   }

   return 0;
}

int TokenRecede(int id ,int *maxTrans, int *currTrans)
{
   int  res ;

   res = TokenQueueOpen(id, &QueueQ, &QueueA);
   if ( res )
   {
      ELOG(ERROR, "´ò¿ªÁîÅÆ¶ÓÁÐÊ§°Ü");
      return res;
   }

   memset(&tokenMsg, 0, sizeof(TTokenMsg));

   tokenMsg.mtype = (long)getpid();
   tokenMsg.rtype = ETOKEN_DECREASE_CURRENT;

   res = MessageQueueSend(QueueQ, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long));
   if ( res )
   {
      ELOG(ERROR, "·¢ËÍÁîÅÆÇëÇóÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   res = MessageQueueRecvEx(QueueA, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long), 5 );
   if ( res )
   {
      ELOG(ERROR, "½ÓÊÜÁîÅÆÓ¦´ðÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   if ( maxTrans )
   {
      *maxTrans = tokenMsg.tmax;
   }

   if ( currTrans )
   {
      *currTrans = tokenMsg.tnum;
   }

   if ( tokenMsg.tcode != ETOKEN_ERR_SUCCESS )
   {
      return tokenMsg.tcode;
   }

   return 0;
}

int TokenInfo(int id ,int *maxTrans, int *currTrans)
{
   int  res ;

   res = TokenQueueOpen(id, &QueueQ, &QueueA);
   if ( res )
   {
      ELOG(ERROR, "´ò¿ªÁîÅÆ¶ÓÁÐÊ§°Ü");
      return res;
   }

   memset(&tokenMsg, 0, sizeof(TTokenMsg));

   tokenMsg.mtype = (long)getpid();
   tokenMsg.rtype = ETOKEN_GET_STATUS;

   res = MessageQueueSend(QueueQ, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long));
   if ( res )
   {
      ELOG(ERROR, "·¢ËÍÁîÅÆÇëÇóÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   res = MessageQueueRecvEx(QueueA, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long), 5 );
   if ( res )
   {
      ELOG(ERROR, "½ÓÊÜÁîÅÆÓ¦´ðÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   if ( maxTrans )
   {
      *maxTrans = tokenMsg.tmax;
   }

   if ( currTrans )
   {
      *currTrans = tokenMsg.tnum;
   }

   if ( tokenMsg.tcode != ETOKEN_ERR_SUCCESS )
   {
      return tokenMsg.tcode;
   }

   return 0;
}

int TokenIncrease(int id ,int *maxTrans, int *currTrans)
{
   int  res ;

   res = TokenQueueOpen(id, &QueueQ, &QueueA);
   if ( res )
   {
      ELOG(ERROR, "´ò¿ªÁîÅÆ¶ÓÁÐÊ§°Ü");
      return res;
   }

   memset(&tokenMsg, 0, sizeof(TTokenMsg));

   tokenMsg.mtype = (long)getpid();
   tokenMsg.rtype = ETOKEN_INCREASE_MAX;

   res = MessageQueueSend(QueueQ, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long));
   if ( res )
   {
      ELOG(ERROR, "·¢ËÍÁîÅÆÇëÇóÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   res = MessageQueueRecvEx(QueueA, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long), 5 );
   if ( res )
   {
      ELOG(ERROR, "½ÓÊÜÁîÅÆÓ¦´ðÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   if ( maxTrans )
   {
      *maxTrans = tokenMsg.tmax;
   }

   if ( currTrans )
   {
      *currTrans = tokenMsg.tnum;
   }

   if ( tokenMsg.tcode != ETOKEN_ERR_SUCCESS )
   {
      return tokenMsg.tcode;
   }

   return 0;
}

int TokenDecrease(int id ,int *maxTrans, int *currTrans)
{
   int  res ;

   res = TokenQueueOpen(id, &QueueQ, &QueueA);
   if ( res )
   {
      ELOG(ERROR, "´ò¿ªÁîÅÆ¶ÓÁÐÊ§°Ü");
      return res;
   }

   memset(&tokenMsg, 0, sizeof(TTokenMsg));

   tokenMsg.mtype = (long)getpid();
   tokenMsg.rtype = ETOKEN_DECREASE_MAX;

   res = MessageQueueSend(QueueQ, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long));
   if ( res )
   {
      ELOG(ERROR, "·¢ËÍÁîÅÆÇëÇóÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   res = MessageQueueRecvEx(QueueA, (TMessageQueueBuffer *)&tokenMsg,sizeof(tokenMsg) - sizeof(long), 5 );
   if ( res )
   {
      ELOG(ERROR, "½ÓÊÜÁîÅÆÓ¦´ðÊ§°Ü,Ê§°ÜÐÅÏ¢(%s:%d)", GetStrError(res), res );
      return res;
   }

   if ( maxTrans )
   {
      *maxTrans = tokenMsg.tmax;
   }

   if ( currTrans )
   {
      *currTrans = tokenMsg.tnum;
   }

   if ( tokenMsg.tcode != ETOKEN_ERR_SUCCESS )
   {
      return tokenMsg.tcode;
   }

   return 0;
}


