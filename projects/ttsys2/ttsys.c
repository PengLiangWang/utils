#include "ttdef.h"
#include <errno.h>


static char  *ttsyserr[] = 
{
   "SUCCESS""TTS_SUCCESS",
   "�ڴ�ռ䲻��""TTS_ENOMEM",
   "���ڹ��ϣ���������Ϣ����""TTS_EGENERAL",
   "��Чֵ""TTS_EINVAL",
   "û��Ȩ��""TTS_EACCES",
   "������ʱ""TTS_TIMEOUT",
   "δ�ҵ�Ŀ�����""TTS_NOTFOUND",
   "�����Ѿ�����""TTS_EXIST",
   "���󲻴���""TTS_ENONE",
   "�򲻿���̬��(dlopen)""TTS_DLOPEN",
   "����Ϊ��""TTS_ENULL",
   "�ļ�����""TTS_EOF"
};

static char  unkown[64];

char * GetStrError(int err)
{
  if ( err == TTS_SUCCESS )
  {
    return "success";
  }
  
  if (err < TTS_MAX_SYS_ERRNO && err > 0)
  {
    return (char *)strerror(err);
  }
  else
  {
    int  erridx = 0 ,max = sizeof(ttsyserr) / sizeof(char *);

    erridx = err - TTS_OS_START_ERROR;
  
    if ( erridx < max )
    {
       return ttsyserr[erridx];
    }
    else
    {
       memset(unkown, 0, sizeof(unkown) );
       snprintf( unkown, sizeof(unkown), "unkown error code %d", err);
       return unkown;
    }
  }
}

