#include "ThreadQueue.h"

uint32 ThreadQueueInfoCreate(TThreadQueueInfo *queueInfo, int maxIdlers)
{
   memset(queueInfo, 0, sizeof(TThreadQueueInfo));

   queueInfo->idlersMutex = PTHREAD_MUTEX_INITIALIZER;
   queueInfo->waitForIdler = PTHREAD_COND_INITIALIZER;
   
   pthread_mutex_init(&queueInfo->idlersMutex, NULL);
   pthread_cond_init(&queueInfo->waitForIdler, NULL);
   queueInfo->maxIdlers = maxIdlers;

   return 0;
}

uint32 ThreadQueueInfoDestroy(TThreadQueueInfo *queueInfo)
{
   pthread_cond_destroy(&queueInfo->waitForIdler);
   pthread_mutex_destroy(&queueInfo->idlersMutex);
   queueInfo->maxIdlers = 0;

   return 0;
}

uint32 ThreadQueueInfoSetIdle(TThreadQueueInfo *queueInfo)
{
   uint32   rv;

   rv = pthread_mutex_lock(&queueInfo->idlersMutex);
   if ( rv )
   {
      return rv;
   }

   if (queueInfo->idlers++ == 0)
   {
      pthread_cond_signal(&queueInfo->waitForIdler);
   }

   rv = pthread_mutex_unlock(&queueInfo->idlersMutex);
   if ( rv )
   {
      return rv;
   }

   return 0;
}

uint32 ThreadQueueInfoWaitIdler(TThreadQueueInfo *queueInfo)
{
   uint32   rv;

   rv = pthread_mutex_lock(&queueInfo->idlersMutex);
   if ( rv )
   {
      return rv;
   }

   while ((queueInfo->idlers == 0) && (!queueInfo->terminated))
   {
      rv = pthread_cond_wait(&queueInfo->waitForIdler,&queueInfo->idlersMutex);
      if ( rv )
      {
         uint32  rv2;

         rv2 = pthread_mutex_unlock(&queueInfo->idlersMutex);
         if ( rv2 )
         {
            return rv2;
         }

         return rv;
      }
   }

   queueInfo->idlers--;

   rv = pthread_mutex_unlock(&queueInfo->idlersMutex);
   if ( rv )
   {
      return rv;
   }
   else if (queueInfo->terminated)
   {
      return THREAD_QUEUE_EOF;
   }
   else
   {
      return 0;
   }
}


uint32 ThreadQueueInfoTerm(TThreadQueueInfo *queueInfo)
{
   uint32   rv;

   rv = pthread_mutex_lock(&queueInfo->idlersMutex);
   if ( rv )
   {
      return rv;
   }

   queueInfo->terminated = 1;
   pthread_cond_broadcast(&queueInfo->waitForIdler);

   rv = pthread_mutex_unlock(&queueInfo->idlersMutex);
   if ( rv )
   {
      return rv;
   }

   return 0;
}

uint32 ThreadQueueDestory(TThreadQueue *queue)
{
   pthread_cond_destroy(&queue->notEmpty);
   pthread_mutex_destroy(&queue->oneMutex);
   free(queue->data);
   return 0;
}

uint32 ThreadQueueInit(TThreadQueue *queue, int capacity)
{
   int    i;

   memset(queue, 0, sizeof(TThreadQueue));

   queue->oneMutex = PTHREAD_MUTEX_INITIALIZER;
   queue->notEmpty = PTHREAD_COND_INITIALIZER;
   
   pthread_mutex_init(&queue->oneMutex, NULL);
   pthread_cond_init(&queue->notEmpty, NULL);
   queue->bounds = capacity;
   queue->nelts = 0;
   queue->data = (TThreadQueueElem *)malloc(sizeof(TThreadQueueElem) * capacity);
   if ( queue->data == NULL )
   {
      pthread_cond_destroy(&queue->notEmpty);
      pthread_mutex_destroy(&queue->oneMutex);
      return -1;
   }
   
   for ( i = 0;  i < capacity; i++)
   {
      queue->data[i].sd = -1;
   }

   return 0;
}

uint32 ThreadQueuePush(TThreadQueue *queue, int sd)
{
   TThreadQueueElem  *elem;
   uint32             rv;

   rv = pthread_mutex_lock(&queue->oneMutex);
   if ( rv )
   {
      return rv;
   }

   if ( ThreadQueueFull(queue) )
   {
      rv = pthread_mutex_unlock(&queue->oneMutex);
      if ( rv )
      {
         return rv;
      }
      if ( queue->terminated )
      {
         return THREAD_QUEUE_EOF;
      }
      else 
      {
         return THREAD_QUEUE_FULL;
      }
   }

   elem = &queue->data[queue->nelts];
   elem->sd = sd;
   queue->nelts++;

   pthread_cond_signal(&queue->notEmpty);

   rv = pthread_mutex_unlock(&queue->oneMutex);
   if ( rv )
   {
      return rv;
   }

   return 0;
}

uint32 ThreadQueuePop(TThreadQueue *queue, int *sd)
{
   uint32   rv;
   TThreadQueueElem  *elem;
   rv = pthread_mutex_lock(&queue->oneMutex);
   if ( rv )
   {
      return rv;
   }

   if ( ThreadQueueEmpty(queue) )
   {
      if (!queue->terminated)
      {
         pthread_cond_wait(&queue->notEmpty, &queue->oneMutex);
      }

      if ( ThreadQueueEmpty(queue) )
      {
         rv = pthread_mutex_unlock(&queue->oneMutex);
         if ( rv )
         {
            return rv;
         }
         if ( queue->terminated )
         {
            return THREAD_QUEUE_EOF;
         }
         else 
         {
            return THREAD_QUEUE_EINTR;
         }
      }
   }

   elem = &queue->data[--queue->nelts];
   *sd = elem->sd;

#ifdef THREAD_QUEUE_DEBUG
   printf("FILE:%s LINE:%d FUNC:%s POP SD SUCCESS\n", __FILE__, __LINE__, __FUNCTION__ );
#endif

   rv = pthread_mutex_unlock(&queue->oneMutex);
   if ( rv )
   {
      return rv;
   }

   return 0;
}

uint32 ThreadQueueInterruptAll(TThreadQueue *queue)
{
   uint32  rv;

   if ((rv = pthread_mutex_lock(&queue->oneMutex)) != 0 )
   {
      return rv;
   }

   pthread_cond_broadcast(&queue->notEmpty);

   if ((rv = pthread_mutex_unlock(&queue->oneMutex)) != 0 )
   {
      return rv;
   }

   return 0;
}

uint32 ThreadQueueTerm(TThreadQueue *queue)
{
   uint32  rv;

   if ((rv = pthread_mutex_lock(&queue->oneMutex)) != 0 )
   {
      return rv;
   }

   queue->terminated = 1;

   if ((rv = pthread_mutex_unlock(&queue->oneMutex)) != 0 )
   {
      return rv;
   }

   return ThreadQueueInterruptAll(queue);
}
