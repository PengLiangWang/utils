#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DesUtil.h"
#include "IBsec.h"

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

int PinHostDes(char *acc,char *key,char *cryMode,char *pin_data,char *pin_end,char flag)
{
  unsigned char data[8];
  unsigned char pin[16];
  int i;
  int n;
  char     acc_no[17];

  memset(acc_no,0,sizeof(acc_no));
  strncpy(acc_no,acc + strlen(acc) - 16,16);

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
        data[n]= (V(pin[i])^V(acc_no[i+1]))<<4 | (V(pin[i+1])^V(acc_no[i+2]));
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
        pin[i] = U((data[n]>>4)^V(acc_no[i+1]));
        pin[i+1] = U((data[n]&0xf)^V(acc_no[i+2]));
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

static int pin_encrypt(char *card_no,char *pin_data,char *cryMode,char *key)
{
  int  rv;
  char pin_end[9];

  memset(pin_end,0,sizeof(pin_end));

  rv = PinHostDes(card_no,key,cryMode,pin_data,pin_end,'E');
  if (rv != 0)
  {
    return rv;
  }
  memset(pin_data,0,8);
  memcpy(pin_data,pin_end,8);
  return 0;
}


static int pin_decrypt(char *card_no,char *pin_data,char *cryMode,char *key)
{
  int  rv;
  char pin_end[9];

  memset(pin_end,0,sizeof(pin_end));

  rv = PinHostDes(card_no,key,cryMode,pin_data,pin_end,'D');
  if (rv != 0)
  {
    return rv;
  }

  memset(pin_data,0,8);
  memcpy(pin_data,pin_end,8);
  
  return 0;
}

int PinAcctCrypt(char crypt_type,char *card_no,char *pin_data,char *old_node,char *new_node,
              int (*getKey)(char *node,char *keyStat,char *cryMode,char *keyA,char *keyB))
{
  char   old_keyStat[2];
  char   old_cryMode[2];
  char   old_keyA[24];
  char   old_keyB[24];
  char  *old_key;
  char   new_keyStat[2];
  char   new_cryMode[2];
  char   new_keyA[24];
  char   new_keyB[24];
  char  *new_key;
  int    rv;
  int    flag = 0;
  char  *env;

  /*
  env = (char *)getenv("IBSECMODE");
  if (env == NULL || strncmp(env,"SERVER",6))
  {
    flag = 1;
  }
  */

  memset(old_keyStat,0,sizeof(old_keyStat));
  memset(old_cryMode,0,sizeof(old_cryMode));
  memset(old_keyA,0,sizeof(old_keyA));
  memset(old_keyB,0,sizeof(old_keyB));

  memset(new_keyStat,0,sizeof(new_keyStat));
  memset(new_cryMode,0,sizeof(new_cryMode));
  memset(new_keyA,0,sizeof(new_keyA));
  memset(new_keyB,0,sizeof(new_keyB));

  if ( crypt_type == PIN_CRYPT_TYPE_ENCRYPT )
  {
    if (flag == 1)
    {
      return -1;
    }

    rv = (*getKey)(new_node,new_keyStat,new_cryMode,new_keyA,new_keyB);
    if (rv != 0)
    {
      return -1;
    }

    switch(new_keyStat[0])
    {
      case IB_SEC_KEY_STAT0:
      case IB_SEC_KEY_STAT2:
        new_key = new_keyA;
        break;
      case IB_SEC_KEY_STAT1:
        new_key = new_keyB;
        break;
      default:
        return -1;
    }

    rv = pin_encrypt(card_no,pin_data,new_cryMode,new_key);
    if (rv != 0)
    {
      return rv;
    }

    return 0;
  }
  else if ( crypt_type == PIN_CRYPT_TYPE_DECRYPT )
  {
    if (flag == 1)
    {
      return -1;
    }

    rv = (*getKey)(old_node,old_keyStat,old_cryMode,old_keyA,old_keyB);
    if (rv != 0)
    {
      return -1;
    }

    switch(old_keyStat[0])
    {
      case IB_SEC_KEY_STAT0:
      case IB_SEC_KEY_STAT2:
        old_key = old_keyA;
        break;
      case IB_SEC_KEY_STAT1:
        old_key = old_keyB;
        break;
      default:
        return -1;
    }

    rv = pin_decrypt(card_no,pin_data,old_cryMode,old_key);
    if (rv != 0)
    {
      return rv;
    }

    return 0;
  }
  else if (crypt_type == PIN_CRYPT_TYPE_HOSTDES)
  {
    rv = (*getKey)(old_node,old_keyStat,old_cryMode,old_keyA,old_keyB);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"getKey");
      return -1;
    }

    switch(old_keyStat[0])
    {
      case IB_SEC_KEY_STAT0:
      case IB_SEC_KEY_STAT2:
        old_key = old_keyA;
        break;
      case IB_SEC_KEY_STAT1:
        old_key = old_keyB;
        break;
      default:
        logger(__FILE__,__LINE__,"old_keyStat");
        return -1;
    }

    rv = pin_decrypt(card_no,pin_data,old_cryMode,old_key);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"pin_decrypt");
      return rv;
    }

    rv = (*getKey)(new_node,new_keyStat,new_cryMode,new_keyA,new_keyB);
    if (rv != 0)
    {      
      logger(__FILE__,__LINE__,"get_key");
      return -1;
    }

    switch(new_keyStat[0])
    {
      case IB_SEC_KEY_STAT0:
      case IB_SEC_KEY_STAT2:
        new_key = new_keyA;
        break;
      case IB_SEC_KEY_STAT1:
        new_key = new_keyB;
        break;
      default:
        logger(__FILE__,__LINE__,"new_keyStat");
        return -1;
    }

    rv = pin_encrypt(card_no,pin_data,new_cryMode,new_key);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"pin_encrpyto");
      return rv;
    }

    return 0;
  }
  else
  {
    return -1;
  }
}
