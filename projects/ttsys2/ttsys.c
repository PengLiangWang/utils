#include "ttdef.h"
#include <errno.h>


static char  *ttsyserr[] = 
{
   "SUCCESS""TTS_SUCCESS",
   "内存空间不足""TTS_ENOMEM",
   "存在故障，但具体信息不详""TTS_EGENERAL",
   "无效值""TTS_EINVAL",
   "没有权限""TTS_EACCES",
   "操作超时""TTS_TIMEOUT",
   "未找到目标对象""TTS_NOTFOUND",
   "对象已经存在""TTS_EXIST",
   "对象不存在""TTS_ENONE",
   "打不开动态库(dlopen)""TTS_DLOPEN",
   "对象为空""TTS_ENULL",
   "文件结束""TTS_EOF"
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

