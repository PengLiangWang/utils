#ifndef _SYS_IB_SEC_UTIL_H_
#define _SYS_IB_SEC_UTIL_H_



#ifdef	__cplusplus
extern	"C"	{
#endif

int PinAcctCrypt(char crypt_type,char *card_no,char *pin_data,char *old_node,char *new_node,
              int (*getKey)(char *node,char *keyStat,char *cryMode,char *keyA,char *keyB));

#ifdef	__cplusplus
}
#endif



#endif
