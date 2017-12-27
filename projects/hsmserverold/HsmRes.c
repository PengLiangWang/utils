#include "ttsys.h"
#include "HsmKeyBox.h"
#include "HsmRes.h"

static TMutex    GMutex;
static TShmem    GShmem;

#define  HSM_SHMEM_SIZE    sizeof(THsmKeyVector) + 12

static THsmKeyVector   *GHsmKeyVector = NULL;

int HsmResGetMutex(TMutex *mutex)
{

  memset(mutex,0,sizeof(mutex));
  memcpy(mutex,&GMutex,sizeof(TMutex));

  return TTS_SUCCESS;
}

int HsmResCreate(int MutexId,int ShmemId,int MsgId)
{
  int        rv;

  rv = MutexCreate(&GMutex,MutexId,1,"HsmServer");
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"创建加密系统的Mutex失败,错误码:%d,错误信息:%s",rv,GetStrError(rv));
    return rv;
  }

  rv = ShmemCreate(&GShmem,HSM_SHMEM_SIZE,ShmemId,0);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"创建加密系统的Shmem失败,错误码:%d,错误信息:%s",rv,GetStrError(rv));
    MutexDestory(&GMutex);
    return rv;
  }

  GHsmKeyVector = (THsmKeyVector *)ShmemGetBaseAddress(&GShmem);
  if ( GHsmKeyVector == NULL )
  {
    ELOG(ERROR,"初始化加密系统时,获取共享内存地址失败");
    MutexDestory(&GMutex);
    ShmemDestory(&GShmem);
    return TTS_ENULL;
  }

  memset(GHsmKeyVector,0,HSM_SHMEM_SIZE);  

  HsmKeyBoxSetVector(GHsmKeyVector);

  return TTS_SUCCESS;
}

int HsmResDestory()
{
  MutexDestory(&GMutex);
  ShmemDestory(&GShmem);
  return TTS_SUCCESS;
}

int HsmResOpen(int MutexId,int ShmemId,int MsgId)
{
  int        rv;

  rv = MutexOpen(&GMutex,MutexId,1,"HsmServer");
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"连接加密系统的Mutex失败,错误码:%d,错误信息:%s",rv,GetStrError(rv));
    return rv;
  }

  rv = ShmemAttach(&GShmem,HSM_SHMEM_SIZE,ShmemId,0);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"连接加密系统的Shmem失败,错误码:%d,错误信息:%s",rv,GetStrError(rv));
    return rv;
  }

  GHsmKeyVector = (THsmKeyVector *)ShmemGetBaseAddress(&GShmem);
  if ( GHsmKeyVector == NULL )
  {
    ELOG(ERROR,"打开加密系统时,获取共享内存地址失败");
    ShmemDetach(&GShmem);
    return TTS_ENULL;
  }

  HsmKeyBoxSetVector(GHsmKeyVector);

  return TTS_SUCCESS;
}

int HsmResClose()
{
  ShmemDetach(&GShmem);
  return TTS_SUCCESS;
}



