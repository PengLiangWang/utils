#ifndef _HSM_COMMAND_H_
#define _HSM_COMMAND_H_

typedef int (*FHsmCommand)(char *cmd,char *inData,int len,char *outData);

struct hsm_command_table
{
  char         command[3];
  char         success[3];
  FHsmCommand  cmdFunc;
  char         stat;
};

typedef struct hsm_command_table THsmCommandTable;

int HsmCommandInit();
int HsmCommandAdd( char *cmd,char *success,FHsmCommand cmdFunc );
int HsmExecCommand(char *cmd,char *inData,int len,char *outData);

#endif



