#include "ttsys.h"
#include "HsmKeyBox.h"
#include "HsmCommand.h"
#include "HsmModule.h"
#include "DesUtil.h"


//生成单个工作密钥 12 + idx(5)  || 13 + 00 + keyDataA(32) + checkVal(8)
static int HsmFunc_12(char *cmd,char *inData,int len,char *outData)
{
  char          binKey[17];
  char          hexKey[33];
  char          strKeyIndex[6];
  char          hexChkV[9];
  int           rv;
  THsmKeyInfo   hsmKeyInfo;

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfo.keyIdx = strtol(strKeyIndex,NULL,10);

  memset(binKey,0,sizeof(binKey));
  memset(hexKey,0,sizeof(hexKey));

  rv = HsmKeyBoxGetKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }
  
  if ( hsmKeyInfo.keyType != '1' && hsmKeyInfo.keyType != '2'  )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  HsmGenWorkKey(hsmKeyInfo.keyDataA,binKey);
  
  memset(hexChkV,0,sizeof(hexChkV));
  HsmGenCheckValue(binKey,hexChkV);
  endesmBlock(hsmKeyInfo.keyDataA,DES_MODE_2,binKey,16);
  binToStr(binKey,hexKey,16);

  strcpy(outData,"00");
  strncat(outData,hexKey,32);
  strncat(outData,hexChkV,8);

  return 0;
}

//生成2个工作密钥 13 + idx(5)  || 14 + 00 + keyDataP(32) + checkVal(8) + keyDataM(32) + checkVal(8)
static int HsmFunc_13(char *cmd,char *inData,int len,char *outData)
{
  char          binKey[17];
  char          hexKey[33];
  char          strKeyIndex[6];
  char          hexChkV[9];
  int           rv;
  THsmKeyInfo   hsmKeyInfo;

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfo.keyIdx = strtol(strKeyIndex,NULL,10);

  
  memset(hsmKeyInfo.keyDataA,0,sizeof(hsmKeyInfo.keyDataA));

  rv = HsmKeyBoxGetKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfo.keyType != '1' && hsmKeyInfo.keyType != '2'  )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  strcpy(outData,"00");

  memset(binKey,0,sizeof(binKey));
  memset(hexKey,0,sizeof(hexKey));
  HsmGenWorkKey(hsmKeyInfo.keyDataA,binKey);
  memset(hexChkV,0,sizeof(hexChkV));
  HsmGenCheckValue(binKey,hexChkV);
  endesmBlock(hsmKeyInfo.keyDataA,DES_MODE_2,binKey,16);

  binToStr(binKey,hexKey,16);
  strncat(outData,hexKey,32);
  strncat(outData,hexChkV,8);

  memset(binKey,0,sizeof(binKey));
  memset(hexKey,0,sizeof(hexKey));
  HsmGenWorkKey(hsmKeyInfo.keyDataA,binKey);
  memset(hexChkV,0,sizeof(hexChkV));
  HsmGenCheckValue(binKey,hexChkV);
  endesmBlock(hsmKeyInfo.keyDataA,DES_MODE_2,binKey,16);
  binToStr(binKey,hexKey,16);
  
  strncat(outData,hexKey,32);
  strncat(outData,hexChkV,8);
  return 0;
}

//转加密 21 + 源idx(5) + 源pinKey(32) + 目标idx(5) + 目标pinKey(32) + pinData(16) + pnd(16) || 22 + 00 + 
static int HsmFunc_21(char *cmd,char *inData,int len,char *outData)
{
  char          binKeyA[17];
  char          binKeyB[17];
  char          hexKey[33];
  char          strKeyIndex[6];
  char          pinData[9];
  char          panData[17];
  char          nPinData[9];
  int           rv;
  THsmKeyInfo   hsmKeyInfoA,hsmKeyInfoB;

  memset(&hsmKeyInfoA, 0, sizeof(THsmKeyInfo));
  memset(&hsmKeyInfoB, 0, sizeof(THsmKeyInfo));

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoA.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoA);
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }
  if ( hsmKeyInfoA.keyType != '1' && hsmKeyInfoA.keyType != '2'  )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(hexKey,0,sizeof(hexKey));
  memcpy(hexKey,inData,32);
  inData += 32;
  strToBin(hexKey,binKeyA,16);

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoB.keyIdx = strtol(strKeyIndex,NULL,10);
  
  rv = HsmKeyBoxGetKey( &hsmKeyInfoB );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }
  if ( hsmKeyInfoB.keyType != '1' && hsmKeyInfoB.keyType != '2'  )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(hexKey,0,sizeof(hexKey));
  memcpy(hexKey,inData,32);
  inData += 32;
  strToBin(hexKey,binKeyB,16);
  
  memset(hexKey,0,sizeof(hexKey));
  memset(pinData,0,sizeof(pinData));
  memcpy(hexKey,inData,16);
  inData += 16;
  strToBin(hexKey,pinData,8);

  memset(panData,0,sizeof(panData));
  memcpy(panData,inData,16);

  dedesmBlock(hsmKeyInfoA.keyDataA,DES_MODE_2,binKeyA,16);
  dedesmBlock(hsmKeyInfoB.keyDataA,DES_MODE_2,binKeyB,16);
  
  memset(nPinData,0,sizeof(nPinData));
  rv = HsmProcessPin(panData,binKeyA,pinData,nPinData,'D');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统解PIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }
  memset(pinData,0,sizeof(pinData));
  rv = HsmProcessPin(panData,binKeyB,nPinData,pinData,'E');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统加密PIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }
  strncat(outData,"00",2);
  binToStr(pinData,outData+2,8);
  return 0;
}

//计算ECB MAC 22 + idx(5) +key+ dataLen(3) + data || 23 + 00 + MAC(16)
static int HsmFunc_22(char *cmd,char *inData,int len,char *outData)
{
  char          binKey[17];
  char          hexKey[33];
  char          strKeyIndex[6];
  char         *macData;
  char          Mac[17];
  int           dataLen;
  int           rv;
  THsmKeyInfo   hsmKeyInfo;

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfo.keyIdx = strtol(strKeyIndex,NULL,10);  

  rv = HsmKeyBoxGetKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfo.keyType != '1' && hsmKeyInfo.keyType != '2'  )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(hexKey,0,sizeof(hexKey));
  memcpy(hexKey,inData,32);
  inData += 32;
  strToBin(hexKey,binKey,16);

  dedesmBlock(hsmKeyInfo.keyDataA,DES_MODE_2,binKey,16);

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,3);
  inData += 3;
  dataLen = strtol(strKeyIndex,NULL,10);

  macData = (char *)malloc(dataLen + 1);
  if ( macData == NULL )
  {
    ELOG(ERROR,"密钥系统分配内存失败");
    strcpy(outData,"96没有足够的内存");
    return TTS_ENOMEM;
  }
  memset(macData,0,dataLen+1);
  
  strToBin(inData,macData,dataLen);

  memset(Mac,0,sizeof(Mac));
  HsmGenMac(macData,dataLen,binKey,Mac);
  free(macData);

  strcpy(outData,"00");
  binToStr(Mac,outData + 2,8);
  return 0;
}

//计算 X99 MAC 23 + idx(5) + dataLen(3) + data || 24 + 00 + MAC(16)
static int HsmFunc_23(char *cmd,char *inData,int len,char *outData)
{
  char          binKey[17];
  char          hexKey[33];
  char          strKeyIndex[6];
  char         *macData;
  char         Mac[17];
  int           dataLen;
  int           rv;
  THsmKeyInfo   hsmKeyInfo;

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfo.keyIdx = strtol(strKeyIndex,NULL,10);  

  rv = HsmKeyBoxGetKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfo.keyType != '1' && hsmKeyInfo.keyType != '2'  )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(hexKey,0,sizeof(hexKey));
  memcpy(hexKey,inData,32);
  inData += 32;
  strToBin(hexKey,binKey,16);

  dedesmBlock(hsmKeyInfo.keyDataA,DES_MODE_2,binKey,16);

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,3);
  inData += 3;
  dataLen = strtol(strKeyIndex,NULL,10);

  macData = (char *)malloc(dataLen + 1);
  if ( macData == NULL )
  {
    ELOG(ERROR,"密钥系统分配内存失败");
    strcpy(outData,"96没有足够的内存");
    return TTS_ENOMEM;
  }
  memset(macData,0,dataLen + 1);
  
  strToBin(inData,macData,dataLen);

  memset(Mac,0,sizeof(Mac));
  HsmGenMacX99(macData,dataLen,binKey,Mac);
  free(macData);

  strcpy(outData,"00");
  binToStr(Mac,outData + 2,8);
  return 0;
}

//转加密 24 + 源idx(5) + 源pinKey(32) + 目标idx(5) + 离散次数 + 离散因子1 + 离散因子2 + pinData(16) + pnd(16) || 25 + 00 + 
static int HsmFunc_24(char *cmd,char *inData,int len,char *outData)
{
  char          binKeyA[17];
  char          strKeyIndex[6];
  char          pinData[9];
  char          panData[17];
  char          nPinData[9];
  int           iNumOfFactor,i;
  int           rv;
  THsmKeyInfo   hsmKeyInfoA,hsmKeyInfoB;

  memset(&hsmKeyInfoA, 0, sizeof(THsmKeyInfo));
  memset(&hsmKeyInfoB, 0, sizeof(THsmKeyInfo));
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoA.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoA );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoA.keyType != '1' && hsmKeyInfoA.keyType != '2' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }
  
  
  memset(binKeyA,0,sizeof(binKeyA));
  strToBin(inData,binKeyA,16);
  inData += 32;
  dedesmBlock(hsmKeyInfoA.keyDataA,DES_MODE_2,binKeyA,16);

  //离散密钥
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoB.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoB );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoB.keyType != '1' && hsmKeyInfoB.keyType != '2' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,1);
  inData += 1;
  iNumOfFactor = strtol(strKeyIndex,NULL,10);
  
  for ( i = 0; i < iNumOfFactor ; i++ )
  {
    memset(pinData,0,sizeof(pinData));
    strToBin(inData,pinData,8);
    inData += 16;
    discreteKey(hsmKeyInfoB.keyDataA,pinData,hsmKeyInfoB.keyDataA);
  }

  memset(pinData,0,sizeof(pinData));
  strToBin(inData,pinData,8);
  inData += 16;
  
  memset(panData,0,sizeof(panData));
  memcpy(panData,inData,16);
  
  memset(nPinData,0,sizeof(nPinData));
  rv = HsmProcessPin(panData,binKeyA,pinData,nPinData,'D');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统解PIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }

  memset(pinData,0,sizeof(pinData));
  rv = HsmProcessPin(panData,hsmKeyInfoB.keyDataA,nPinData,pinData,'E');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统加密PIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }
  strncat(outData,"00",2);
  binToStr(pinData,outData+2,8);
  return 0;
}

//转加密 25 + 源idx(5) + 离散次数 + 离散因子1 + 离散因子2 + 目标idx(5) + 源pinKey(32) + pinData(16) + pnd(16) || 26 + 00 + 
static int HsmFunc_25(char *cmd,char *inData,int len,char *outData)
{
  char          binKeyB[17];
  char          strKeyIndex[6];
  char          pinData[9];
  char          panData[17];
  char          nPinData[9];
  int           iNumOfFactor,i;
  int           rv;
  THsmKeyInfo   hsmKeyInfoA,hsmKeyInfoB;

  memset(&hsmKeyInfoA, 0, sizeof(THsmKeyInfo));
  memset(&hsmKeyInfoB, 0, sizeof(THsmKeyInfo));
  //获取主密钥
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5); //act
  inData += 5;
  hsmKeyInfoA.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoA );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoA.keyType != '1' && hsmKeyInfoA.keyType != '2' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  //离散主密钥
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,1);  //act
  inData += 1;
  iNumOfFactor = strtol(strKeyIndex,NULL,10);
  for ( i = 0; i < iNumOfFactor ; i++ )
  {
    memset(pinData,0,sizeof(pinData));
    strToBin(inData,pinData,8);  //act
    inData += 16;
    discreteKey(hsmKeyInfoA.keyDataA,pinData,hsmKeyInfoA.keyDataA);
  }

  //主密钥
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoB.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoB );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoB.keyType != '1' && hsmKeyInfoB.keyType != '2' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }
  
  memset(binKeyB,0,sizeof(binKeyB));
  strToBin(inData,binKeyB,16);
  inData += 32;
  dedesmBlock(hsmKeyInfoB.keyDataA,DES_MODE_2,binKeyB,16);


  memset(pinData,0,sizeof(pinData));
  strToBin(inData,pinData,8);
  inData += 16;
  
  memset(panData,0,sizeof(panData));
  memcpy(panData,inData,16);
  
  memset(nPinData,0,sizeof(nPinData));
  rv = HsmProcessPin(panData,hsmKeyInfoA.keyDataA,pinData,nPinData,'D');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统解PIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }

  memset(pinData,0,sizeof(pinData));
  rv = HsmProcessPin(panData,binKeyB,nPinData,pinData,'E');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统加密PIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }
  strncat(outData,"00",2);
  binToStr(pinData,outData+2,8);
  return 0;
}

//解密 26 + 目标idx(5) + 目标pinKey(32) + pinData(16) + pnd(16) || 27 + 00 + 
static int HsmFunc_26(char *cmd,char *inData,int len,char *outData)
{
  char          binKeyA[17];
  char          hexKey[33];
  char          strKeyIndex[6];
  char          pinData[9];
  char          panData[17];
  char          nPinData[9];
  int           rv;
  THsmKeyInfo   hsmKeyInfoA;

  memset(&hsmKeyInfoA, 0, sizeof(THsmKeyInfo));

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoA.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoA );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoA.keyType != '1' && hsmKeyInfoA.keyType != '2' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(hexKey,0,sizeof(hexKey));
  memcpy(hexKey,inData,32);
  inData += 32;
  strToBin(hexKey,binKeyA,16);
  
  memset(hexKey,0,sizeof(hexKey));
  memset(pinData,0,sizeof(pinData));
  memcpy(hexKey,inData,16);
  inData += 16;
  strToBin(hexKey,pinData,8);

  memset(panData,0,sizeof(panData));
  memcpy(panData,inData,16);

  dedesmBlock(hsmKeyInfoA.keyDataA,DES_MODE_2,binKeyA,16);
  
  memset(nPinData,0,sizeof(nPinData));
  rv = HsmProcessPin(panData,binKeyA,pinData,nPinData,'E');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统解PIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }

  strncat(outData,"00",2);
  binToStr(nPinData,outData+2,8);
  return 0;
}



#include <openssl/rsa.h>

//用户密码加密 A1 + 密钥索引(5) + (用户号码长度)用户号码(16位) + 密码数据(256) + 目标密钥索引(5) | A2 + 00 + 密钥数据(64)
static int HsmFunc_A1(char *cmd,char *inData,int len,char *outData)
{
  char          strKeyIndex[6];
  int           rv;
  THsmKeyInfo   hsmKeyInfoA,hsmKeyInfoB;
  RSA          *rsa;
  uchar         rsaN[129],rsaD[129];
  uchar         userId[25];
  int           dataLen;
  uchar         enData[129],deData[129];
  uchar         newEnData[129];
  uchar         userPwd[20];

  memset(&hsmKeyInfoA, 0, sizeof(THsmKeyInfo));
  memset(rsaN, 0, sizeof(rsaN));
  memset(rsaD, 0, sizeof(rsaD));
  memset(strKeyIndex,0,sizeof(strKeyIndex));

  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoA.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoA );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoA.keyType != '3' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(strKeyIndex, 0, sizeof(strKeyIndex));
  memcpy(strKeyIndex, inData, 2);
  inData += 2;
  dataLen = strtol(strKeyIndex,NULL,10);

  memset(userId, 0, sizeof(userId) );
  memcpy(userId, inData, dataLen);
  inData += dataLen;

  memset(strKeyIndex, 0, sizeof(strKeyIndex));
  memcpy(strKeyIndex, inData, 3);
  inData += 3;
  dataLen = strtol(strKeyIndex,NULL,10);
  memset(enData, 0, sizeof(enData) );
  strToBin(inData, enData, dataLen);
  inData += dataLen * 2;


  rsa = RSA_new();
  if ( rsa == NULL )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  rsa->d = BN_bin2bn( (uchar *)hsmKeyInfoA.keyDataA, hsmKeyInfoA.keyLen, rsa->d );
  rsa->n = BN_bin2bn( (uchar *)hsmKeyInfoA.keyDataB, hsmKeyInfoA.keyLen, rsa->n );
  rsa->e = BN_bin2bn( "\x01\x00\x01", 3, rsa->e );

  memset(deData, 0, sizeof(deData) );

  rv = RSA_private_decrypt( dataLen, enData, deData, rsa, RSA_PKCS1_PADDING );
  if ( rv == -1 )
  {
    ELOG(ERROR,"密钥系统%d的密钥解密失败",hsmKeyInfoA.keyIdx);
    strcpy(outData,"96解密失败");
    RSA_free( rsa );
    return TTS_EGENERAL;
  }

  RSA_free( rsa );
  
  //密码数据格式为 00000000 + 08(密码长度) + 12345678 + 日期(8) + 时间(6)
  if ( memcmp(deData, "00000000", 8 ) != 0 )
  {
    ELOG(ERROR,"密钥系统%d的密钥解密后数据非法",hsmKeyInfoA.keyIdx);
    strcpy(outData,"96数据非法");
    return TTS_EGENERAL;
  }

  memset(strKeyIndex, 0, sizeof(strKeyIndex));
  memcpy(strKeyIndex, deData + 8 , 2);
  dataLen = strtol(strKeyIndex,NULL,10);
  memset(userPwd, 0, sizeof(userPwd) );
  memcpy(userPwd, deData + 10, dataLen);

  memset(enData, 0, sizeof(enData) );
  memset(newEnData,0,sizeof(newEnData));

  snprintf(newEnData, sizeof(newEnData), "%02d%s%02d%s", strlen(userId), userId, strlen(userPwd), userPwd);  
  snprintf(enData, 32, "%032s", newEnData);
  dataLen = 32;
 
  ELOG(ERROR,"RSADATA[%s][%s]", enData,newEnData);

  memset(strKeyIndex, 0, sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoB.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoB );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoB.keyType != '2' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  endesmBlock(hsmKeyInfoB.keyDataA, DES_MODE_2, enData, dataLen);

  strncat(outData,"00",2);
  
  binToStr(enData,outData + 2,dataLen);

  return 0;
}


//银行卡密码加密 B1 + 密钥索引(5) + 订单号(16位) + 密码数据(256) + 目标密钥索引(5) | B2 + 00 + 密钥数据(16)
static int HsmFunc_B1(char *cmd,char *inData,int len,char *outData)
{
  char          strKeyIndex[6];
  int           rv;
  THsmKeyInfo   hsmKeyInfoA,hsmKeyInfoB;
  RSA          *rsa;
  uchar         rsaN[129],rsaD[129];
  int           dataLen;
  char          binKeyB[17];
  char          pinData[9];
  char          panData[17];
  char          nPinData[9];
  uchar         enData[129],deData[129];



  memset(&hsmKeyInfoA, 0, sizeof(THsmKeyInfo));
  memset(rsaN, 0, sizeof(rsaN));
  memset(rsaD, 0, sizeof(rsaD));
  
  memset(strKeyIndex,0,sizeof(strKeyIndex));

  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoA.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoA );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoA.keyType != '3' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  memset(panData, 0, sizeof(panData));
  memcpy(panData, inData, 16);
  inData += 16;

  memset(strKeyIndex, 0, sizeof(strKeyIndex));
  memcpy(strKeyIndex, inData, 3);
  inData += 3;
  dataLen = strtol(strKeyIndex,NULL,10);
  memset(enData, 0, sizeof(enData) );
  strToBin(inData, enData, dataLen);
  inData += dataLen * 2;

  rsa = RSA_new();
  if ( rsa == NULL )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  rsa->d = BN_bin2bn( (uchar *)hsmKeyInfoA.keyDataA, hsmKeyInfoA.keyLen, rsa->d );
  rsa->n = BN_bin2bn( (uchar *)hsmKeyInfoA.keyDataB, hsmKeyInfoA.keyLen, rsa->n );
  rsa->e = BN_bin2bn( "\x01\x00\x01", 3, rsa->e );

  memset(deData, 0, sizeof(deData) );

  rv = RSA_private_decrypt( dataLen, enData, deData, rsa, RSA_PKCS1_PADDING );
  if ( rv == -1 )
  {
    ELOG(ERROR,"密钥系统%d的密钥解密失败",hsmKeyInfoA.keyIdx);
    strcpy(outData,"96解密失败");
    RSA_free( rsa );
    return TTS_EGENERAL;
  }

  RSA_free( rsa );
  
  //deData 为银行卡密码信息  FFFFFFFF + PAN + 123456 + 

  if ( memcmp(deData, "FFFFFFFF", 8 ) != 0 )
  {
    ELOG(ERROR,"密钥系统%d的密钥解密后数据非法(PIN数据头信息不正确)",hsmKeyInfoA.keyIdx);
    strcpy(outData,"96数据非法");
    return TTS_EGENERAL;
  }

  if ( memcmp(deData + 8 , panData, 16 ) != 0 )
  {
    ELOG(ERROR,"密钥系统%d的密钥解密后数据非法(PAN数据不正确)",hsmKeyInfoA.keyIdx);
    strcpy(outData,"96数据非法");
    return TTS_EGENERAL;
  }

  memset(pinData, 0, sizeof(pinData));
  memcpy(pinData, deData + 24, 6);


  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoB.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfoB );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfoA.keyType != '2' || hsmKeyInfoA.keyType != '1' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoB.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }
  
  memset(binKeyB, 0, sizeof(binKeyB));
  memcpy(binKeyB, "20120708",8);

  discreteKey(hsmKeyInfoB.keyDataA,binKeyB,hsmKeyInfoB.keyDataA);

  memset(panData, 0, sizeof(panData));
  memcpy(panData,"0000000000000000",16);

  memset(nPinData,0,sizeof(nPinData));
  rv = HsmProcessPin(panData, hsmKeyInfoB.keyDataA, pinData, nPinData, 'E');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统加密USERPIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }
  strncat(outData,"00",2);
  binToStr(nPinData,outData+2,8);
  return 0;

}

//银行卡密码加密 B2 + 密钥索引(5) + 密码数据(256) + FACTTORA + FACTTORB + FACTTORC | B2 + 00 + 密钥数据(16)
static int HsmFunc_B2(char *cmd,char *inData,int len,char *outData)
{
  char          strKeyIndex[6];
  int           rv;
  THsmKeyInfo   hsmKeyInfoA;
  RSA          *rsa;
  uchar         rsaN[129],rsaD[129];
  int           dataLen;
  char          binKeyB[17];
  char          pinData[9];
  char          panData[17];
  char          nPinData[9];
  uchar         enData[129],deData[129];
  char          factorA[17],factorB[17],factorC[17];

  ELOG(ERROR, "INTO HSMFUNC_B2");

  memset(&hsmKeyInfoA, 0, sizeof(THsmKeyInfo));
  memset(strKeyIndex,0,sizeof(strKeyIndex));

  memcpy(strKeyIndex,inData,5);
  inData += 5;
  hsmKeyInfoA.keyIdx = strtol(strKeyIndex,NULL,10);

  ELOG(ERROR, "HSMKEYBOXGETKEY start");

  rv = HsmKeyBoxGetKey( &hsmKeyInfoA );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  ELOG(ERROR, "HSMKEYBOXGETKEY end");

  if ( hsmKeyInfoA.keyType != '2' &&  hsmKeyInfoA.keyType != '1' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfoA.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  ELOG(ERROR, "everything is OK!");

  memset(strKeyIndex, 0, sizeof(strKeyIndex));
  memcpy(strKeyIndex, inData, 2);
  inData += 2;
  dataLen = strtol(strKeyIndex,NULL,10);
  memset(pinData, 0, sizeof(pinData) );
  memcpy(pinData, inData, dataLen);
  inData += dataLen;

  dataLen = 8;
  memset(factorA,0,sizeof(factorA));
  strToBin(inData, factorA, dataLen);
  inData += 16;
  memset(factorB,0,sizeof(factorB));
  strToBin(inData, factorB, dataLen);
  inData += 16;
  memset(factorC,0,sizeof(factorC));
  strToBin(inData, factorC, dataLen);
  inData += 16;

  discreteKey(hsmKeyInfoA.keyDataA, factorA, hsmKeyInfoA.keyDataA);
  discreteKey(hsmKeyInfoA.keyDataA, factorB, hsmKeyInfoA.keyDataA);
  discreteKey(hsmKeyInfoA.keyDataA, factorC, hsmKeyInfoA.keyDataA);

  memset(panData, 0, sizeof(panData));
  memcpy(panData,"0000000000000000",16);

  memset(nPinData,0,sizeof(nPinData));

  ELOG(ERROR, "HSMPROCESSPIN start");

  rv = HsmProcessPin(panData, hsmKeyInfoA.keyDataA, pinData, nPinData, 'E');
  if ( rv )
  {
    ELOG(ERROR,"密钥系统加密USERPIN失败,返回码:%d",rv);
    strcpy(outData,"01无效PIN数据");
    return rv;
  }

  ELOG(ERROR, "HSMPROCESSPIN end");

  strncat(outData,"00",2);
  binToStr(nPinData,outData+2,8);

  return 0;
}

static THsmCommandTable GHsmCommandTable[11] = 
{
  {"12","13",HsmFunc_12,'Y'},
  {"13","14",HsmFunc_13,'Y'},
  {"21","22",HsmFunc_21,'Y'},
  {"22","23",HsmFunc_22,'Y'},
  {"23","24",HsmFunc_23,'Y'},
  {"24","25",HsmFunc_24,'Y'},
  {"25","26",HsmFunc_25,'Y'},
  {"26","27",HsmFunc_26,'Y'},
  {"A1","A2",HsmFunc_A1,'Y'},
  {"B1","B2",HsmFunc_B1,'Y'},
  {"B2","B3",HsmFunc_B2,'Y'}
};

THsmModule HsmCommand1 = 
{
  11,
  &GHsmCommandTable[0]
};
