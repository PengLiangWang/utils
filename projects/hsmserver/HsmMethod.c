#include "HsmKeyBox.h"
#include "DesUtil.h"
#include "ttsys.h"

int HsmGenCheckValue(char  *mkey,char *checkVal)
{
  char   tmpData[9];
  char   hexChkV[17];

  memset(tmpData,0x00,sizeof(tmpData));
  memset(hexChkV,0x00,sizeof(hexChkV));

  endesmBlock( mkey,DES_MODE_2,tmpData , 8 );

  binToStr(tmpData,hexChkV,8);
  strncpy(checkVal,hexChkV,8);
  return 0;
}


static int GRandIdx = 0;

static int HsmSetRand()
{
  long   t;

  time(&t);
  srand( t + GRandIdx );
  if ( GRandIdx++ > 999999 )
  {
    GRandIdx = 1;
  }
  
  return 0;
}

static int HsmGetRandBuf(char  *buf)
{
  int    iRand1,iRand2,iRand3;
  char   strRand[33];
  long   t;
  time(&t);

  HsmSetRand();
  iRand1 = rand();

  HsmSetRand();
  iRand2 = rand();

  HsmSetRand();
  iRand3 = rand();

  sprintf(strRand,"%06d%05d%06d%06d%09d",iRand1,(int)getpid(),iRand2,iRand3,t);
  memcpy(buf,strRand,32);
  return 0;
}

int HsmGenMaterKey(char *mKeyA,char *mKeyB,char *mKey)
{
  char  strRand[33];
  char  strKey[33];
  char  strBuff[64];
  int   i;

  memset(strRand,0,sizeof(strRand));
  HsmGetRandBuf(strRand);
  sprintf(strBuff,"1A2CB3E4F5607189%032sFFFFFFFFFFFFFFFF",strRand);
  MDString1(strBuff,strKey);
  memcpy(mKeyA,strKey,32);

  memset(strRand,0,sizeof(strRand));
  memset(strKey,0,sizeof(strKey));
  HsmGetRandBuf(strRand);
  sprintf(strBuff,"F1AFF2CF%016sFBF3FE4FFF5FF60F%016sF71FF8F9",strRand,strRand+16);
  MDString1(strBuff,strKey);
  memcpy(mKeyB,strKey,32);

  memset(strRand,0,sizeof(strRand));
  memset(strKey,0,sizeof(strKey));

  strToBin(mKeyA,strRand,16);
  strToBin(mKeyB,strKey,16);

  for ( i = 0; i< 16;i++ )
  {
    mKey[i] = strRand[i] ^ strKey[i];
  }

  return 0;
}

int HsmGenWorkKey(char *mKey,char *wKey)
{
  char  strRand[33];
  char  strKey[33];
  char  strBuff[64];

  memset(strRand,0,sizeof(strRand));
  HsmGetRandBuf(strRand);
  sprintf(strBuff,"1A2CB3E4F5607189%032sFFFFFFFFFFFFFFFF",strRand);
  MDString1(strBuff,strKey);
  memset(strRand,0,sizeof(strRand));
  strToBin(strKey,strRand,16);
  endesmBlock(mKey,DES_MODE_2,strRand,16);
  memcpy(wKey,strRand,16);
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

int HsmProcessPin(char *acc,char *key,char *pin_data,char *pin_end,char flag)
{
  unsigned char data[8];
  unsigned char pin[16];
  int i;
  int n;

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

      endesm(key,DES_MODE_2,(char *)data);
      memcpy(pin_end,data,8);
      break;
    case 'D':
    case 'd':
      memset(pin,0,sizeof(pin));
      memset(data,0,sizeof(data));
      memcpy(data,pin_data,8);
      dedesm(key,DES_MODE_2,(char *)data);
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


int HsmGenMac(char *data,int len,char *key,char *mac)
{
  int  i,j,k ;
  char tbuf0[9];
  char tbuf1[17];
  char tbuf2[9];
  char *macsourse;

  memset(tbuf0,0,sizeof(tbuf0));
  memset(tbuf2,0,sizeof(tbuf2));
  memset(tbuf1,0,sizeof(tbuf1));

  j = len % 8 ? (len + ( 8 - len % 8 )) / 8 : len / 8;

  macsourse = (char *)malloc(j * 8);
  memset(macsourse,0x00,j * 8);
  memcpy(macsourse,data,len);
  memset(macsourse + len ,0x00,j * 8 - len);
  
  for (i = 0;i < j ;i++ )
  {
    for (k = 0;k < 8 ;k++ )
    {
      tbuf0[k] ^= macsourse[k + i * 8];
    }
  }

  free(macsourse);
  
  binToStr(tbuf0,tbuf1,8);
  
  memcpy(tbuf0,tbuf1,8);


  endesm(key,DES_MODE_2,tbuf0);

  for (k = 0;k < 8 ; k++)
  {
    tbuf0[k] ^= tbuf1[8 + k];
  }

  endesm(key,DES_MODE_2,tbuf0);
  
  
  memcpy(mac,tbuf0,8);

  return 0;

}

int HsmGenMacX99(char *data,int len,char *key,char *mac)
{
  return desmCreatMACX99(key,DES_MODE_2,data,len,mac);
}
