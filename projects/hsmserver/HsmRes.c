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
    ELOG(ERROR,"��������ϵͳ��Mutexʧ��,������:%d,������Ϣ:%s",rv,GetStrError(rv));
    return rv;
  }

  rv = ShmemCreate(&GShmem,HSM_SHMEM_SIZE,ShmemId,0);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"��������ϵͳ��Shmemʧ��,������:%d,������Ϣ:%s",rv,GetStrError(rv));
    MutexDestory(&GMutex);
    return rv;
  }

  GHsmKeyVector = (THsmKeyVector *)ShmemGetBaseAddress(&GShmem);
  if ( GHsmKeyVector == NULL )
  {
    ELOG(ERROR,"��ʼ������ϵͳʱ,��ȡ�����ڴ��ַʧ��");
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
    ELOG(ERROR,"���Ӽ���ϵͳ��Mutexʧ��,������:%d,������Ϣ:%s",rv,GetStrError(rv));
    return rv;
  }

  rv = ShmemAttach(&GShmem,HSM_SHMEM_SIZE,ShmemId,0);
  if ( rv != TTS_SUCCESS )
  {
    ELOG(ERROR,"���Ӽ���ϵͳ��Shmemʧ��,������:%d,������Ϣ:%s",rv,GetStrError(rv));
    return rv;
  }

  GHsmKeyVector = (THsmKeyVector *)ShmemGetBaseAddress(&GShmem);
  if ( GHsmKeyVector == NULL )
  {
    ELOG(ERROR,"�򿪼���ϵͳʱ,��ȡ�����ڴ��ַʧ��");
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



