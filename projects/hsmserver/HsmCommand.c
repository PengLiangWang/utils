#include "ttsys.h"
#include "HsmCommand.h"


static THsmCommandTable  GHsmCommand[257];


static int HsmFunc_00(char *cmd,char *inData,int len,char *outData)
{
  strcpy(outData,"00");
  return 0;
}

int HsmCommandAdd( char *cmd,char *success,FHsmCommand cmdFunc )
{
  int               cmdIdx;
  THsmCommandTable *table;

  if ( cmd == NULL || strlen(cmd) != 2 || success == NULL || strlen(success) != 2 )
  {
    return TTS_EINVAL;
  }

  cmdIdx = (int)strtol(cmd,NULL,16);
  if ( cmdIdx > 0xFF || cmdIdx < 0x00 )
  {
    return TTS_EINVAL;
  }

  ELOG(INFO, "支持指令: %s",cmd);

  table = &GHsmCommand[cmdIdx];
  strcpy(table->command,cmd);
  strcpy(table->success,success);
  table->cmdFunc = cmdFunc;
  table->stat = 'Y';

  return 0;
}


int HsmExecCommand(char *cmd,char *inData,int len,char *outData)
{
  int               cmdIdx;
  THsmCommandTable *table;

  if ( cmd == NULL || strlen(cmd) != 2 )
  {
    strcpy(outData,"XXXX无效指令");
    return TTS_EINVAL;
  }

  cmdIdx = (int)strtol(cmd,NULL,16);
  if ( cmdIdx > 0xFF || cmdIdx < 0x00 )
  {
    strcpy(outData,"XXXX无效指令");
    return TTS_EINVAL;
  }

  table = &GHsmCommand[cmdIdx];
  
  if ( table->stat != 'Y' )
  {
    strcpy(outData,"XXXX无效指令");
    return TTS_EINVAL;
  }

  strncpy(outData,table->success,2);

  return table->cmdFunc(cmd,inData,len,outData + 2);
}

#include "HsmKeyBase.c"

int HsmCommandInit()
{
  memset(&GHsmCommand,0,sizeof(GHsmCommand));
  HsmCommandAdd("01","02",HsmFunc_01);
  HsmCommandAdd("02","03",HsmFunc_02);
  HsmCommandAdd("03","04",HsmFunc_03);
  HsmCommandAdd("04","05",HsmFunc_04);
  HsmCommandAdd("05","06",HsmFunc_05);
  return HsmCommandAdd("00","01",HsmFunc_00);
}
