#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IBsecQueue.h"
#include "DesUtil.h"

int IBsecInit()
{
  desInit(0);

  return sec_queue_init(0);
}

int IBsecGetSecNode(char *node_id,int maxlen)
{
  IBCMDINFO kc;
  int       rv;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_GET_SEC_NODE;

  rv = sec_queue_cmd(&kc);
  if (rv != 0)
  {
    return rv;
  }

  strncpy(node_id,kc.info.key.node_id,maxlen);
  
  return 0;
}

int IBsecGetKeyAllByC(char *node_id,char *type,char *keyState,char *cryMode,char *keyA,char *keyB,char *keyC)
{
  IBCMDINFO kc;
  int       rv;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_GET_C;
  strncpy(kc.info.key.node_id,node_id,10);
  strncpy(kc.info.key.key_type,type,1);
  memmove(kc.info.key.keyC,keyC,16);
  
  rv = sec_queue_cmd(&kc);
  if (rv != 0)
  {
    return rv;
  }

  memmove(keyState,kc.info.key.key_state,2);
  memmove(keyA,kc.info.key.keyA,16);
  memmove(keyB,kc.info.key.keyB,16);
  memmove(cryMode,kc.info.key.cry_mode,2);

  return 0;
}

int IBsecGetKeyByC(char *node_id,char *type,char *keyState,char *keyA,char *keyB,char *keyC)
{
  char tmpmode[2];

  return IBsecGetKeyAllByC(node_id,type,keyState,tmpmode,keyA,keyB,keyC);
}

int IBsecGetKeyAll(char *node_id,char *type,char *keyState,char *cryMode,char *keyA,char *keyB)
{
  IBCMDINFO kc;
  int       rv;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_GET;

  strncpy(kc.info.key.node_id,node_id,10);
  strncpy(kc.info.key.key_type,type,1);
  
  rv = sec_queue_cmd(&kc);
  if (rv != 0)
  {
    return rv;
  }
  
  memmove(keyState,kc.info.key.key_state,2);
  memmove(cryMode,kc.info.key.cry_mode,2);
  memmove(keyA,kc.info.key.keyA,16);
  memmove(keyB,kc.info.key.keyB,16);

  return 0;
}

int IBsecGetKey(char *node_id,char *type,char *keyState,char *keyA,char *keyB)
{
  char tmpmode[2];

  return IBsecGetKeyAll(node_id,type,keyState,tmpmode,keyA,keyB);
}

int IBsecAddKeyAllByC(char *node_id,char *type, char *keyState,char *cryMode,char *keyA,char *keyB,char *keyC)
{
  IBCMDINFO kc;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_ADD;
  strncpy(kc.info.key.node_id,node_id,10);
  strncpy(kc.info.key.key_type,type,1);
  memmove(kc.info.key.key_state,keyState,2);
  memmove(kc.info.key.cry_mode,cryMode,2);
  memmove(kc.info.key.keyA,keyA,16);
  memmove(kc.info.key.keyB,keyB,16);
  memmove(kc.info.key.keyC,keyC,16);

  return sec_queue_cmd(&kc);
}

int IBsecAddKeyByC(char *node_id,char *type,char *keyState,char *keyA,char *keyB,char *keyC)
{
  char mode[2];
  
  mode[0] = DES_MODE_2;
  mode[1] = '\0';
  
  return IBsecAddKeyAllByC(node_id,type,keyState,mode,keyA,keyB,keyC);
}

int IBsecSetKeyAllByC(char *node_id,char *type,char *keyState,char *cryMode,char *keyA,char *keyB,char *keyC)
{
  IBCMDINFO kc;
  int       rv;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_UPDATE;
  strncpy(kc.info.key.node_id,node_id,10);
  strncpy(kc.info.key.key_type,type,1);
  memmove(kc.info.key.key_state,keyState,2);
  memmove(kc.info.key.cry_mode,cryMode,2);
  memmove(kc.info.key.keyA,keyA,16);
  memmove(kc.info.key.keyB,keyB,16);
  memmove(kc.info.key.keyC,keyC,16);
  
  return sec_queue_cmd(&kc);
}

int IBsecLockKey(char *node_id,char *type)
{
  IBCMDINFO  kc;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_LOCK;
  strncpy(kc.info.key.node_id,node_id,10);
  strncpy(kc.info.key.key_type,type,1);
  return sec_queue_cmd(&kc);
}

int IBsecUnlockKey(char *node_id,char *type)
{
  IBCMDINFO  kc;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_UNLOCK;
  strncpy(kc.info.key.node_id,node_id,10);
  strncpy(kc.info.key.key_type,type,1);

  return sec_queue_cmd(&kc);
}

int IBsecDelKeyByC(char *node_id,char *type,char *keyC)
{
  IBCMDINFO  kc;
  
  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_DEL;
  strncpy(kc.info.key.node_id,node_id,10);
  strncpy(kc.info.key.key_type,type,1);
  memmove(kc.info.key.keyC,keyC,16);
  return sec_queue_cmd(&kc);
}

int IBsecChangePA(char *old,char *new)
{
  IBCMDINFO  kc;
  
  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_CHANGE_PA;
  memmove(kc.info.key.keyA,old,16);
  memmove(kc.info.key.keyB,new,16);

  return sec_queue_cmd(&kc);
}

int IBsecChangePB(char *old,char *new)
{
  IBCMDINFO  kc;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_CHANGE_PB;
  memmove(kc.info.key.keyA,old,16);
  memmove(kc.info.key.keyB,new,16);
  return sec_queue_cmd(&kc);
}

int IBsecCheckPA(char *passwdA)
{
  IBCMDINFO  kc;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_CHECK_PA;
  memmove(kc.info.key.keyA,passwdA,16);

  return sec_queue_cmd(&kc);
}

int IBsecCheckPB(char *passwdB)
{
  IBCMDINFO  kc;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_KEY_CHECK_PB;
  memmove(kc.info.key.keyB,passwdB,16);
  return sec_queue_cmd(&kc);
}

int IBsecEncryptPin(char *node_id,char *card_no,char *pin_data)
{
  IBCMDINFO  kc;
  int        rv;
  char       *env;

  env = (char *)getenv("IBSECMODE");
  if (env == NULL || strncmp(env,"SERVER",6))
  {
    return 1;
  }
  
  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_CRYPT_PIN;
  kc.info.pin.crypt_type[0] = PIN_CRYPT_TYPE_ENCRYPT;
  
  //strncpy(kc.info.pin.node_id,node_id,10);
  strncpy(kc.info.pin.new_node_id,"CARDPIN",7);
  strncpy(kc.info.pin.new_node_id,node_id,10);
  strncpy(kc.info.pin.account_no,card_no,19);
  memmove(kc.info.pin.pin_data,pin_data,8 );
  
  rv = sec_queue_cmd(&kc);
  
  if (rv)
  {
    return rv;
  }
  
  memmove(pin_data,kc.info.pin.pin_data,8);

  return rv;  
}

int IBsecDecryptPin(char *node_id,char *card_no,char *pin_data)
{
  IBCMDINFO  kc;
  int        rv;
  char       *env;

  env = (char *)getenv("IBSECMODE");
  if (env == NULL || strncmp(env,"SERVER",6))
  {
    return 1;
  }

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_CRYPT_PIN;
  kc.info.pin.crypt_type[0] = PIN_CRYPT_TYPE_DECRYPT;

  strncpy(kc.info.pin.node_id,"CARDPIN",7);
  //strncpy(kc.info.pin.node_id,node_id,10);
  strncpy(kc.info.pin.new_node_id,node_id,10);
  strncpy(kc.info.pin.account_no,card_no,19);
  memmove(kc.info.pin.pin_data,pin_data,8);
  
  rv = sec_queue_cmd(&kc);
  if (rv)
  {
    return rv;
  }
  
  memmove(pin_data,kc.info.pin.pin_data,8);

  return rv;  
}

int IBsecExchangePin(char *old_node_id,char *new_node_id,char *card_no,char *pin_data)
{
  IBCMDINFO  kc;
  int        rv;

  memset(&kc,0,sizeof(kc));
  kc.cmd = IBCMD_CRYPT_PIN;
  kc.info.pin.crypt_type[0] = PIN_CRYPT_TYPE_HOSTDES;

  strncpy(kc.info.pin.node_id,old_node_id,10);
  strncpy(kc.info.pin.new_node_id,new_node_id,10);
  strncpy(kc.info.pin.account_no,card_no,19);
  memmove(kc.info.pin.pin_data,pin_data,8);
  rv = sec_queue_cmd(&kc);
  if (rv)
  {
    return rv;
  }
  memmove(pin_data,kc.info.pin.pin_data,8);
  return rv;  
}

int IBsecGetMacKey(char *node_id,char *keyState,char *keyA,char *keyB)
{
  char tmptype[2];
  char tmpmode[2];

  tmptype[0] = KEYTYPE_MAC;
  tmptype[1] = 0;

  return IBsecGetKeyAll(node_id,tmptype,keyState,tmpmode,keyA,keyB);
}

int IBsecGetPkgKey(char *node_id,char *keyState,char *keyA,char *keyB)
{
  char tmptype[2];
  char tmpmode[2];

  tmptype[0] = KEYTYPE_PKG;
  tmptype[1] = 0;

  return IBsecGetKeyAll(node_id,tmptype,keyState,tmpmode,keyA,keyB);
}

int IBsecGetPinData(char *node_id,char *card_no,char *in_pin_data,int in_pin_format,char *out_pin_data)
{
  IBCMDINFO  kc;
  int        rv;

  memset(&kc,0,sizeof(kc));

  kc.cmd = IBCMD_CRYPT_PIN;

  kc.info.pin.crypt_type[0] = PIN_CRYPT_TYPE_HOSTDES;

  strncpy(kc.info.pin.node_id,node_id,10);
  strncpy(kc.info.pin.new_node_id,"CARDPIN",7);
  strncpy(kc.info.pin.account_no,card_no,19);
  
  if (in_pin_format == PIN_FORMAT_HEX)
  {
    strToBin(kc.info.pin.pin_data,in_pin_data,8);
  }
  else if (in_pin_format == PIN_FORMAT_BIN)
  {
    memmove(kc.info.pin.pin_data,in_pin_data,8);
  }
  else
  {
    logger(__FILE__,__LINE__,"in_pin_format");
    return -1;
  }
  
  rv = sec_queue_cmd(&kc);
  if (rv)
  {
    logger(__FILE__,__LINE__,"sec_queue_cmd");
    return rv;
  }
  
  binToStr(out_pin_data,kc.info.pin.pin_data,8);
  
  return 0;
}

int IBsecCreateMac(char *node_id,char *block,int len,char *mac)
{
  char   keyState[2];
  char   keyA[17],keyB[17];
  char  *key;
  int    rv;

  memset(keyState,0,sizeof(keyState));
  memset(keyA,0,sizeof(keyA));
  memset(keyB,0,sizeof(keyB));

  rv = IBsecGetMacKey(node_id,keyState,keyA,keyB);
  if (rv)
  {
    return rv;
  }

  switch (keyState[0])
  {
    case IB_SEC_KEY_STAT0:
    case IB_SEC_KEY_STAT1:
      key = keyA;
      break;
    case IB_SEC_KEY_STAT2:
      key = keyB;
      break;
    default:
      return -1;
  }

  desmCreatMAC(key,DES_MODE_2,block,len,mac);

  return 0;
}

int IBsecCheckMac(char *node_id,char *block,int len,char *mac)
{
  char   keyState[2];
  char   keyA[17],keyB[17];
  char  *key;
  int    rv;
  char   checkMac[9];

  memset(keyState,0,sizeof(keyState));
  memset(keyA,0,sizeof(keyA));
  memset(keyB,0,sizeof(keyB));
  memset(checkMac,0,sizeof(checkMac));

  rv = IBsecGetMacKey(node_id,keyState,keyA,keyB);
  if (rv)
  {
    return rv;
  }

  switch (keyState[0])
  {
    case IB_SEC_KEY_STAT0:
    case IB_SEC_KEY_STAT1:
      key = keyA;
      break;
    case IB_SEC_KEY_STAT2:
      key = keyB;
      break;
    default:
      return -1;
  }

  desmCreatMAC(key,DES_MODE_2,block,len,checkMac);

  if (strncmp(checkMac,mac,8))
  {
    return 1;
  }

  return 0;
}

int IBsecEndesBlock(char *node_id,char *block,int len)
{
  char   keyState[2];
  char   keyA[17],keyB[17];
  char  *key;
  int    rv;
  
  rv = IBsecGetPkgKey(node_id,keyState,keyA,keyB);
  if (rv)
  {
    return rv;
  }

  switch (keyState[0])
  {
    case IB_SEC_KEY_STAT0:
    case IB_SEC_KEY_STAT1:
      key = keyA;
      break;
    case IB_SEC_KEY_STAT2:
      key = keyB;
      break;
    default:
      return -1;
  }

  endesmBlock(key,DES_MODE_2,block,len);

  return 0;
}

int IBsecDedesBlock(char *node_id,char *block,int len)
{
  char   keyState[2];
  char   keyA[17],keyB[17];
  char  *key;
  int    rv;
  
  rv = IBsecGetPkgKey(node_id,keyState,keyA,keyB);
  if (rv)
  {
    return rv;
  }

  switch (keyState[0])
  {
    case IB_SEC_KEY_STAT0:
    case IB_SEC_KEY_STAT1:
      key = keyA;
      break;
    case IB_SEC_KEY_STAT2:
      key = keyB;
      break;
    default:
      return -1;
  }

  dedesmBlock(key,DES_MODE_2,block,len);

  return 0;
}


static char V(unsigned char c)
{
  if (c<'0'||c>'9')
    return((unsigned char)0x0f);
  return((unsigned char)(c-'0'));
}
static char U(unsigned char c)
{
  if (c==0xf)
    return('\0');
  if (c>0x9)
    return('E');
  return((unsigned char)(c+'0'));
}

int PinHostDes(char *acc_no,char *key,char *cryMode,char *pin_data,char *pin_end,char flag)
{
 unsigned char data[8];
 unsigned char pin[16];
 int i;
 int n;
 char   acc[17];

 memset(acc,0,sizeof(acc));
 
 strncpy(acc,acc_no + strlen(acc_no) - 16,16);

 switch(flag)
 {
   case 'E':
   case 'e':
     memset(pin,0,sizeof(pin));
     memset(data,0,sizeof(data));
     memcpy((char *)pin,pin_data,8);
     data[0] = strlen(pin_data);
     data[1] = V(pin[0])<<4 | V(pin[1]);
     for (n = 2,i = 2; n < 8; n++, i += 2)
       data[n]= (V(pin[i])^V(acc[i+1]))<<4 | (V(pin[i+1])^V(acc[i+2]));
     endesm(key,cryMode[0],(char *)data);
     memcpy(pin_end,data,8);
     break;
   case 'D':
   case 'd':
     memset(pin,0,sizeof(pin));
     memset(data,0,sizeof(data));
     memcpy(data,pin_data,8);
     dedesm(key,cryMode[0],(char *)data);
     if (data[0] < 0x04 || data[0] >0x0C)
       return -1;

     pin[0]=U(data[1]>>4);
     pin[1]=U(data[1]&0xf);
     for(n=2,i=2; n<8; n++,i+=2)
     {
       pin[i] = U((data[n]>>4)^V(acc[i+1]));
       pin[i+1] = U((data[n]&0xf)^V(acc[i+2]));
     }

     for (i = (int)data[0]; i < 16; ++i)
     {
       if (pin[i] != '\0')
         return -2;
     }
     memcpy(pin_end,pin,8);
     break;
   default:
     return -3;
 }
 return 0;
}


int IBsecExchangeUserPin(char *mk,char *pk,char *pin,char *card_no,char *node_id,int flag)
{
  char   masterKey[9];
  char   workKey[9];
  char   pin_data[9];
  char   cryMode[2];
  int    rv;

  memset(masterKey,0,sizeof(masterKey));
  memset(workKey,0,sizeof(workKey));
  memset(pin_data,0,sizeof(pin_data));

  strToBin(masterKey,mk,8);
  strToBin(workKey,pk,8);
  cryMode[0] = DES_MODE_1;
  cryMode[1] = 0;

  dedesm(masterKey,DES_MODE_1,workKey);

  if(flag == 0)
  {
    rv = PinHostDes("0000000000000000",workKey,cryMode,pin,pin_data,'D');
  }
  else 
  {
    rv = PinHostDes(card_no,workKey,cryMode,pin,pin_data,'D');
  }
  if(rv)
  {
    logger(__FILE__,__LINE__,"PIN¸ñÊ½´í!");
    return -1;
  }
   
  rv = IBsecEncryptPin(node_id,card_no,pin_data);
  if(rv)
  {
    return -1;
  }

  memset(pin,0,8);
  memcpy(pin,pin_data,8);
  return 0;
}
