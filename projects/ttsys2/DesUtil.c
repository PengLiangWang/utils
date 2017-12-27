#include <stdio.h>
#include <string.h>
#include "DesUtil.h"

int desInit(int mode)
{
  return(0);
}
void desdone()
{
  return ;
}

/* Set key (initialize key schedule array) */
int desSetKey(char *key,DesKeyS *kn)
{
  des_cblock  *p;
  p=(des_cblock *)key;
  kn->desmode=DES_MODE_1;
  des_set_key(p,kn->ks);
  return 0;
}

int desmSetKey(char *key,char DesMode,DesKeyS *kn)
{
  des_cblock  *p;
  p=(des_cblock *)key;
  kn->desmode=DesMode;
  switch(DesMode)
  {
    case DES_MODE_0 :   break;
    case DES_MODE_1 :   des_set_key(p,kn->ks); break;

    case DES_MODE_2 :   
          des_set_key(p,kn->ks); 
          des_set_key(p+1,kn->ks2);break;

    case DES_MODE_3 :   des_set_key(p,kn->ks);
          des_set_key(p+1,kn->ks2);
                des_set_key(p+2,kn->ks3); break;

    default:    des_set_key(p,kn->ks);return -1;
  }
  return 0;
}

/* In-place encryption of 64-bit block */
int endesKeyS(DesKeyS *kn,char *block)
{


  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks,DES_ENCRYPT);
  return 0;
}


int endesmKeyS(DesKeyS *kn,char *block)
{

  switch(kn->desmode)
  { 
  case DES_MODE_0 :
  break;
  case DES_MODE_1 :
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks,DES_ENCRYPT);
  break;
  case DES_MODE_2:
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks,DES_ENCRYPT);
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks2,DES_DECRYPT);
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
      kn->ks,DES_ENCRYPT);
  break;

  
  case DES_MODE_3:
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks,DES_ENCRYPT);
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks2,DES_DECRYPT);
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
      kn->ks3,DES_ENCRYPT);
  break;



  default :
  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks,DES_ENCRYPT);
  return -1;  
  }


  return 0;
}

/* In-place decryption of 64-bit block. This function is the mirror
 * image of encryption; exactly the same steps are taken, but in
 * reverse order
 */
int dedesKeyS(DesKeyS *kn,char *block)
{

  des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
            kn->ks,DES_DECRYPT);
  return 0;
}

int dedesmKeyS(DesKeyS *kn,char *block)
{

  switch(kn->desmode)
  { 
    case DES_MODE_0 :
      break;
    case DES_MODE_1 :
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
              kn->ks,DES_DECRYPT);
      break;
    case DES_MODE_2:
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
              kn->ks,DES_DECRYPT);
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
              kn->ks2,DES_ENCRYPT);
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
          kn->ks,DES_DECRYPT);
      break;

    
    case DES_MODE_3:
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
              kn->ks3,DES_DECRYPT);
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
              kn->ks2,DES_ENCRYPT);
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
          kn->ks,DES_DECRYPT);
      break;


    default :
      des_ecb_encrypt((des_cblock *)block,(des_cblock *)block,
              kn->ks,DES_DECRYPT);
    return -1;
  }


  return 0;
}


int endes(char *key,char *block)
{
  DesKeyS   ks;

  desSetKey(key,&ks);
  return(endesKeyS(&ks,block));
}

int endesm(char *key,char desmode,char *block)
{
  DesKeyS   ks;

  desmSetKey(key,desmode,&ks);
  return(endesmKeyS(&ks,block));
}


int dedes(char *key,char *block)
{
  DesKeyS   ks;

  desSetKey(key,&ks);
  return(dedesKeyS(&ks,block));
}

int dedesm(char *key,char mode,char *block)
{
  DesKeyS   ks;

  desmSetKey(key,mode,&ks);
  return(dedesmKeyS(&ks,block));
}
int endesBlockKeyS(DesKeyS  *ks,char *data,int len)
{
  int i,j;
  int dlen;
  int slen;

  slen=len%8;
  dlen=len-slen;

  for(i=0; i<dlen; i+=8)
  {
    /*
    if (i>0)
      for(j=0; j<8; ++j)
        data[i+j]^=data[i+j-8];
    */
    endesKeyS(ks,data+i);
  };
  if ((len>8)&&(slen!=0))
    endesKeyS(ks,data+len-8);
  return(len);
}

int endesmBlockKeyS(DesKeyS  *ks,char *data,int len)
{
  int i,j;
  int dlen;
  int slen;
  if(ks->desmode=='0')
    return 0;

  slen=len%8;
  dlen=len-slen;

  for(i=0; i<dlen; i+=8)
  {
    /*
    if (i>0)
      for(j=0; j<8; ++j)
        data[i+j]^=data[i+j-8];
    */
    endesmKeyS(ks,data+i);
  };
  if ((len>8)&&(slen!=0))
    endesmKeyS(ks,data+len-8);
  return(len);
}




int dedesBlockKeyS(DesKeyS *ks,char *data,int len)
{
  int i,j;
  int dlen;
  int slen;


  slen=len%8;
  dlen=len-slen;

  if ((len>8)&&(slen!=0))
    dedesKeyS(ks,data+len-8);
  for(i=dlen-8; i>=0; i-=8)
  { 
    dedesKeyS(ks,data+i);
    /*
    if (i>0)
      for(j=0; j<8; ++j)
        data[i+j]^=data[i+j-8];
    */
  };
  return(len);
}
int dedesmBlockKeyS(DesKeyS *ks,char *data,int len)
{
  int i,j;
  int dlen;
  int slen;


  slen=len%8;
  dlen=len-slen;

  if(ks->desmode=='0')
    return 0;

  if ((len>8)&&(slen!=0))
    dedesmKeyS(ks,data+len-8);
  for(i=dlen-8; i>=0; i-=8)
  { 
    dedesmKeyS(ks,data+i);
    /*
    if (i>0)
      for(j=0; j<8; ++j)
        data[i+j]^=data[i+j-8];
    */
  };
  return(len);
}
int desCreatMACKeyS(DesKeyS *ks,char *data,int len,char *mac)
{
  int   i,j;
  char  result[17];

  memset(mac,0,8);
  memset(result,0,sizeof(result));

  for(i=0; i<len; i+=8)
  {
    for(j=0; (j<8)&&((i+j)<len); ++j)
      mac[j]^=data[i+j];
  }

  binToStr(mac,result,8);
  memset(mac,0,8);
  memcpy(mac,result,8);
  endesKeyS(ks,mac);
  for (j = 0;j < 8 ;j++ )
  {
    mac[j] ^= result[8 + j];
  }
  endesKeyS(ks,mac);
  memset(result,0,sizeof(result));
  binToStr(mac,result,8);
  memset(mac,0,8);
  memcpy(mac,result,8);

  return(0);
}

int desmCreatMACKeyS(DesKeyS *ks,char *data,int len,char *mac)
{
  int   i,j;
  char  result[17];

  memset(mac,0,8);
  memset(result,0,sizeof(result));

  for(i=0; i<len; i+=8)
  {
    for(j=0; (j<8)&&((i+j)<len); ++j)
      mac[j]^=data[i+j];
  }

  binToStr(mac,result,8);

  memset(mac,0,8);
  memcpy(mac,result,8);
  endesmBlockKeyS(ks,mac,8);
  
  for (j = 0;j < 8 ;j++ )
  {
    mac[j] ^= result[8 + j];
  }
  endesmBlockKeyS(ks,mac,8);
  memset(result,0,sizeof(result));
  binToStr(mac,result,8);
  memset(mac,0,8);
  memcpy(mac,result,8);
  
  return(0);
}

int desmCreatMACX99KeyS(DesKeyS *ks,char *data,int len,char *mac)
{
 int   i,j;
 char  result[17];

 memset(mac,0,8);
 memset(result,0,sizeof(result));

 for(i=0; i<len; i+=8)
 {
   for(j=0; (j<8)&&((i+j)<len); ++j)
     mac[j]^=data[i+j];
   endesmBlockKeyS(ks,mac,8);
 }
 return(0);
}


int desmCreat24MACKeyS(DesKeyS *ks,char *data,int len,char *mac)
{
  int i,j;

  memset(mac,0,24);
  for(i=0; i<len; i+=24)
  {
    for(j=0; (j<24)&&((i+j)<len); ++j)
        mac[j]^=data[i+j];
    endesmBlockKeyS(ks,mac,24);
  };
  return(0);
}

int desCreatMAC(char *key,char *block,int len,char *mac)
{
  DesKeyS   ks;

  desSetKey(key,&ks);
  return(desCreatMACKeyS(&ks,block,len,mac));
}

int desmCreatMAC(char *key,char mode,char *block,int len,char *mac)
{
  DesKeyS   ks;
  desmSetKey(key,mode,&ks);
  return(desmCreatMACKeyS(&ks,block,len,mac));
}

int desmCreatMACX99(char *key,char mode,char *block,int len,char *mac)
{
  DesKeyS   ks;
  desmSetKey(key,mode,&ks);
  return(desmCreatMACX99KeyS(&ks,block,len,mac));
}

int desmCreat24MAC(char *key,char mode,char *block,int len,char *mac)
{
  DesKeyS   ks;

  desmSetKey(key,mode,&ks);
  return(desmCreat24MACKeyS(&ks,block,len,mac));
}

int endesBlock(char *key,char *block,int len)
{
  DesKeyS   ks;
  desSetKey(key,&ks);
  return(endesBlockKeyS(&ks,block,len));
}

int endesmBlock(char *key,char mode,char *block,int len)
{
  DesKeyS   ks;
  desmSetKey(key,mode,&ks);
  return(endesmBlockKeyS(&ks,block,len));
}

int dedesBlock(char *key,char *block,int len)
{
  DesKeyS   ks;

  desSetKey(key,&ks);
  return(dedesBlockKeyS(&ks,block,len));
}

int dedesmBlock(char *key,char mode,char *block,int len)
{
  DesKeyS   ks;

  desmSetKey(key,mode,&ks);
  return(dedesmBlockKeyS(&ks,block,len));
}

int discreteKey(char *key,char *factor,char *newkey)
{
  char      data[17];
  DesKeyS   ks;
  int       i;

  desmSetKey(key,DES_MODE_2,&ks);
  memset(data,0,sizeof(data));
  memcpy(data,factor,8);

  for (i=0;i<8 ;i++ )
  {
    data[i+8] = data[i] ^ 0xFF;
  }

  endesmBlockKeyS( &ks,data , 16 );
  memcpy(newkey,data,16);
  return 0;
}

