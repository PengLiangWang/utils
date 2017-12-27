#ifndef _SYS_IB_SEC_KEY_DATA_H_
#define _SYS_IB_SEC_KEY_DATA_H_

#define ENV_IBSECKEYDB        "IBSECKEYDB"

/*
 ( node     char  15
 * type     char  1
 ( keystat  char  1
 * cryMode  char  1
 ( keyA     char  48
 * keyB     char  48
 ( mark     char  16
 */

int OpenSecKeyFile();
int OpenSecKeyFileAndClean();
int CloseSecKeyFile();
int WriteSecKeyM(char *node,char *mark);
int WriteSecKeyList(int n,char *node,char *type,char *keyState,
                    char *cryMode,char *keyA,char *keyB);
int DeleteSecKeyList(int n,char *node,char *type);
int LoadSecKeyFile(int (*lkm)(char *node,char *mark),
                   int (*lk)(int n,char *node,char *keyType,
                             char *keyState,char *cryMode,
                             char *keyA,char *keyB));

#endif
