#ifndef _HSM_RESOURCES_H_
#define _HSM_RESOURCES_H_

#include "ttsys.h"

int HsmResGetMutex(TMutex *mutex);

int HsmResCreate(int MutexId,int ShmemId,int MsgId);

int HsmResDestory();

int HsmResOpen(int MutexId,int ShmemId,int MsgId);

int HsmResClose();


#endif





