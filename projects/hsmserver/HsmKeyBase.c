#include "ttsys.h"
#include "HsmKeyBox.h"
#include "HsmCommand.h"
#include "HsmModule.h"
#include "DesUtil.h"


//注入加主密钥 01 + idx(5) + keyData(32)  || 11 + 00 + checkVal(8)
static int HsmFunc_01(char *cmd,char *inData,int len,char *outData)
{
  char          binKey[17];
  char          strKeyIndex[6];
  char          hexChkV[9];
  int           iKeyIndex;
  int           rv;
  THsmKeyInfo   hsmKeyInfo;
  
  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  iKeyIndex = strtol(strKeyIndex,NULL,10);

  memset(binKey,0,sizeof(binKey));
  strToBin(inData,binKey,16);
  
  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  hsmKeyInfo.keyIdx  = iKeyIndex;
  hsmKeyInfo.keyType = '2';
  hsmKeyInfo.keyFlag = '2';
  hsmKeyInfo.keyLen  = 16;
  
  memcpy(hsmKeyInfo.keyDataA, binKey, 16);
  memcpy(hsmKeyInfo.keyDataB, binKey, 16);

  rv = HsmKeyBoxUpdateKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"更新密钥系统序号为%d的密钥失败,返回码%d",iKeyIndex,rv);
    strcpy(outData,"96更新密钥失败");
    return rv;
  }
  
  memset(hexChkV,0,sizeof(hexChkV));
  HsmGenCheckValue(binKey,hexChkV);

  strcpy(outData,"00");
  strcat(outData,hexChkV);

  return 0;
}


//生成主密钥 02 + idx(5)  || 12 + 00 + keyDataA(32) + checkVal(8) + keyDataA(32) + checkVal(8) + checkVal(8)
static int HsmFunc_02(char *cmd,char *inData,int len,char *outData)
{
  char          binKey[17];
  char          hexKeyA[33];
  char          hexKeyB[33];
  char          strKeyIndex[6];
  char          hexChkA[9];
  char          hexChkB[9];
  char          hexChkV[9];
  int           iKeyIndex;
  int           rv;
  THsmKeyInfo   hsmKeyInfo;

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  iKeyIndex = strtol(strKeyIndex,NULL,10);

  memset(binKey,0,sizeof(binKey));
  memset(hexKeyA,0,sizeof(hexKeyA));
  memset(hexKeyB,0,sizeof(hexKeyB));

  HsmGenMaterKey(hexKeyA,hexKeyB,binKey);

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  hsmKeyInfo.keyIdx  = iKeyIndex;
  hsmKeyInfo.keyType = '2';
  hsmKeyInfo.keyFlag = '2';
  hsmKeyInfo.keyLen  = 16;

  memcpy(hsmKeyInfo.keyDataA, binKey, 16);
  memcpy(hsmKeyInfo.keyDataB, binKey, 16);

  rv = HsmKeyBoxUpdateKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"更新密钥系统序号为%d的密钥失败,返回码%d",iKeyIndex,rv);
    strcpy(outData,"96更新密钥失败");
    return rv;
  }

  memset(hexChkV,0,sizeof(hexChkV));
  HsmGenCheckValue(binKey,hexChkV);

  memset(hexChkA,0,sizeof(hexChkA));
  memset(binKey,0,sizeof(binKey));
  strToBin(hexKeyA,binKey,16);
  HsmGenCheckValue(binKey,hexChkA);

  memset(hexChkB,0,sizeof(hexChkB));
  memset(binKey,0,sizeof(binKey));
  strToBin(hexKeyB,binKey,16);
  HsmGenCheckValue(binKey,hexChkB);

  strcpy(outData,"00");
  strncat(outData,hexKeyA,32);
  strncat(outData,hexChkA,8);
  strncat(outData,hexKeyB,32);
  strncat(outData,hexChkB,8);
  strncat(outData,hexChkV,8);

  return 0;
}

#include <openssl/rsa.h>

//生成RSA主密钥 03 + idx(5)  || 04 + 00 + keyDataA(32)
static int HsmFunc_03(char *cmd,char *inData,int len,char *outData)
{
  char          strKeyIndex[6];
  int           iKeyIndex,iKeyLen;
  int           rv;
  THsmKeyInfo   hsmKeyInfo;
  RSA          *rsa;
  uchar         rsaN[129],rsaD[129];
  char          strRsaN[257];
  

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;
  iKeyIndex = strtol(strKeyIndex,NULL,10);


  memset(rsaN, 0, sizeof(rsaN));
  memset(rsaD, 0, sizeof(rsaD));

  rsa = RSA_generate_key( 1024, RSA_F4, NULL, NULL );
  if ( rsa == NULL )
  {
    ELOG(ERROR,"产生%d的密钥失败",iKeyIndex);
    strcpy(outData,"96产生新密钥失败");
    return TTS_EGENERAL;
  }

  BN_bn2bin( rsa->n, rsaN ); // 保存公钥
  BN_bn2bin( rsa->d, rsaD ); // 保存私钥
  
  iKeyLen = RSA_size( rsa );

  RSA_free( rsa );

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  hsmKeyInfo.keyIdx  = iKeyIndex;
  hsmKeyInfo.keyType = '3';
  hsmKeyInfo.keyFlag = '2';
  hsmKeyInfo.keyLen  = iKeyLen;
  
  memcpy(hsmKeyInfo.keyDataA, rsaD, iKeyLen);
  memcpy(hsmKeyInfo.keyDataB, rsaN, iKeyLen);

  rv = HsmKeyBoxUpdateKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"更新密钥系统序号为%d的密钥失败,返回码%d",iKeyIndex,rv);
    strcpy(outData,"96更新密钥失败");
    return rv;
  }

  memset(strRsaN, 0, sizeof(strRsaN));
  
  binToStr(rsaN,strRsaN,iKeyLen);

  strcpy(outData,"00");
  strncat(outData,strRsaN, iKeyLen * 2);

  return 0;
}

//获取RSA公钥 04 + idx(5) || 05 + 00 + keyDataA(32)
static int HsmFunc_04(char *cmd,char *inData,int len,char *outData)
{
  char          strKeyIndex[6];
  int           rv;
  THsmKeyInfo   hsmKeyInfo;
  char          strRsaN[257];
  

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  hsmKeyInfo.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }

  if ( hsmKeyInfo.keyType != '3' )
  {
    ELOG(ERROR,"密钥系统%d的密钥类型不正确,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return TTS_EGENERAL;
  }

  strcpy(outData,"00");
  binToStr(hsmKeyInfo.keyDataB, outData + 2, hsmKeyInfo.keyLen);

  return 0;
}

//锁住主密钥 05 + idx(5) || 06 + 00 + 0000
static int HsmFunc_05(char *cmd,char *inData,int len,char *outData)
{
  char          strKeyIndex[6];
  int           rv;
  THsmKeyInfo   hsmKeyInfo;
  char          strRsaN[257];
  

  memset(strKeyIndex,0,sizeof(strKeyIndex));
  memcpy(strKeyIndex,inData,5);
  inData += 5;

  memset(&hsmKeyInfo, 0, sizeof(THsmKeyInfo));
  hsmKeyInfo.keyIdx = strtol(strKeyIndex,NULL,10);

  rv = HsmKeyBoxGetKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"查找密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96无主密钥");
    return rv;
  }
  
  if ( hsmKeyInfo.keyFlag == 1 )
  {
    strcpy(outData,"00");
    strncat(outData, "0000", 4);
    return 0;
  }
  else
  {
    hsmKeyInfo.keyFlag = '1';
    hsmKeyInfo.keyStat = 'L';
  }

  rv = HsmKeyBoxUpdateKey( &hsmKeyInfo );
  if ( rv )
  {
    ELOG(ERROR,"更新密钥系统序号为%d的密钥失败,返回码%d",hsmKeyInfo.keyIdx,rv);
    strcpy(outData,"96锁定密钥失败");
    return rv;
  }

  strcpy(outData,"00");
  strncat(outData, "0000", 4);

  return 0;
}




