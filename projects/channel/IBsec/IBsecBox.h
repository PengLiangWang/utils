#ifndef _SYS_IB_SEC_BOX_H_
#define _SYS_IB_SEC_BOX_H_

#ifdef	__cplusplus
extern	"C"	{
#endif

extern char  MKEY[];

int IBsecBoxInit();
int IBsecBoxLoadMKey(char *dbname,char *dbpasswd,char *keyA,char *keyB);
int IBsecBoxSaveMKey(char *dbname,char *dbpasswd,char *keyA,char *keyB);
int IBsecBoxClean();


#ifdef	__cplusplus
}
#endif

#endif
