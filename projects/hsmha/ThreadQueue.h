#ifndef _HSMHA_THREAD_QUEUE_H_
#define _HSMHA_THREAD_QUEUE_H_

#include <pthread.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int   uint32;

#define THREAD_QUEUE_EOF    100014
#define THREAD_QUEUE_EINTR  100015
#define THREAD_QUEUE_FULL   100016

struct thread_queue_info_t
{
   int              idlers;
   int              maxIdlers;
   int              terminated;
   pthread_mutex_t  idlersMutex;
   pthread_cond_t   waitForIdler;
};

typedef struct thread_queue_info_t TThreadQueueInfo;


struct thread_queue_elem_t
{
   int    sd;
};

struct thread_queue_t
{
   struct thread_queue_elem_t  *data;
   int                          nelts;
   int                          bounds;
   int                          terminated;
   pthread_mutex_t              oneMutex;
   pthread_cond_t               notEmpty;
};

typedef struct thread_queue_elem_t  TThreadQueueElem;

typedef struct thread_queue_t  TThreadQueue;


#define ThreadQueueFull(queue) ((queue)->nelts == (queue)->bounds)

#define ThreadQueueEmpty(queue) ((queue)->nelts == 0)

#endif




