#ifndef _HSM_CONFIG_H_
#define _HSM_CONFIG_H_



struct hsm_server_config
{
  int       level;
  int       listenPort;
  int       minWorker;
  int       maxWorker;
  int       shmemId;
  int       mutexId;
  int       msgId;
  char      logFile[33];
  char      dbUser[33];
  char      dbName[33];
  char      dbPswd[33];
};

typedef struct hsm_server_config  THsmServerConfig;

#define HSM_SERVER_CONF_FILE        "hsm.ini"

int HsmLoadConfig(char *fileName,THsmServerConfig *confData);

#include "ttsys.h"

int HsmLoadCommand(char *fileName,int *count,TMemPool *MemPool);

#endif




