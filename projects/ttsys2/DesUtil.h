#ifndef _DES_UTIL_INCLUDE_H_
#define _DES_UTIL_INCLUDE_H_

#include <openssl/des.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DES_MODE_0 '0'
#define DES_MODE_1 '1'
#define DES_MODE_2 '2'
#define DES_MODE_3 '3'

typedef struct
{
  des_key_schedule  ks;
  des_key_schedule  ks2;
  des_key_schedule  ks3;
  char      desmode;
}DesKeyS;


// en ¼Ó  de ½â

int desInit(int mode);
int desSetKey(char *key,DesKeyS *keyS);
int desmSetKey(char *key,char cryMode,DesKeyS *keyS);


int endes(char *key,char *data);
int endesm(char *key,char cryMode,char *data);
int dedes(char *key,char *data);
int dedesm(char *key,char cryMode,char *data);

int endesBlock(char *key,char *data,int len);
int endesmBlock(char *key,char cryMode,char *data,int len);

int dedesBlock(char *key,char *data,int len);
int dedesmBlock(char *key,char cryMode,char *data,int len);

int desCreatMAC(char *key,char *data,int len,char *mac);
int desmCreatMAC(char *key,char cryMode,char *data,int len,char *mac);

int endesKeyS(DesKeyS *key,char *data);
int endesmKeyS(DesKeyS *key,char *data);

int dedesKeyS(DesKeyS *key,char *data);
int dedesmKeyS(DesKeyS *key,char *data);

int endesBlockKeyS(DesKeyS *key,char *data,int len);
int dedesBlockKeyS(DesKeyS *key,char *data,int len);
int dedesmBlockKeyS(DesKeyS *key,char *data,int len);
int endesmBlockKeyS(DesKeyS *key,char *data,int len);

int desCreatMACKeyS(DesKeyS *key,char *data,int len,char *mac);
int desmCreatMACKeyS(DesKeyS *key,char *data,int len,char *mac);

int discreteKey(char *key,char *factor,char *newkey);

#ifdef __cplusplus
}
#endif

#endif
