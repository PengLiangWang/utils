#include <stdio.h>
#include <string.h>
#include "IBnote.h"
#include "note_error.h"

int IBGetTrans(char *msgType,char *transCode,char *dataType,char *transMode,
      int *timeout,char *group,char *program,char *summary)
{
  int     r;
  char    str[30];
  char    msgType_[5];
  char    transCode_[7];
  char    transMode_[2];
  char    dataType_[2];
  int     timeout_;
  char    timeout_s[10];
  char    group_s[10];
  char    program_[32];
  char    summary_[40];

  r = note_v_index(IB_TRANS_INFO,msgType,transCode,NULL);
  if(r)
  {
    logger(__FILE__,__LINE__,"Index error r=%d",r);
    return r;
  }
  r = note_v_get(2,dataType_,2,transMode_,10,timeout_s,10,group_s,
      31,program_,39,summary_,0);
  if(r != 0 && r != NOTE_ENDOFDATA)
  {
    logger(__FILE__,__LINE__,"Get [%s][%s] TRANS Err[%d]",msgType,transCode,r);
    return r;
  }

  if (dataType!=NULL)
    strncpy(dataType,dataType_,2);
  if (transMode!=NULL)
    strncpy(transMode,transMode_,2);
  if (timeout!=NULL)
    *timeout=atoi(timeout_s);
  if (group!=NULL)
  {
    strncpy(group,group_s,9);
  }
  if (program!=NULL)
    strncpy(program,program_,32);
  if (summary!=NULL)
    strncpy(summary,summary_,11);
  return(0);
}

int IBGetTransMode(char *msgType,char *transCode,char *transMode)
{
  return(IBGetTrans(msgType,transCode,NULL,transMode,NULL,NULL,NULL,
      NULL));
}


int IBGetTransNoGroup(char *msgType,char *transCode,char *dataType,
      char *transMode,int *timeout,char *prg,char *summary)
{
  return(IBGetTrans(msgType,transCode,dataType,transMode,timeout,NULL,
      prg,summary));
}

int IBGetTransDataType(char *msgType,char *transCode,char *dataType)
{
  return(IBGetTrans(msgType,transCode,dataType,NULL,NULL,NULL,
      NULL,NULL));
}

int IBGetTransTimeout(char *msgType,char *transCode,int *timeout)
{
  return(IBGetTrans(msgType,transCode,NULL,NULL,timeout,NULL,
      NULL,NULL));
}

int IBGetTransProgram(char *msgType,char *transCode,char *transMode,char *program,char *group)
{
  return(IBGetTrans(msgType,transCode,NULL,transMode,NULL,group,
      program,NULL));
}

int IBGetTransSummary(char *msgType,char *transCode,char *summary)
{
  return(IBGetTrans(msgType,transCode,NULL,NULL,NULL,NULL,
      NULL,summary));
}
