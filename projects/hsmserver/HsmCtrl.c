#include "ttsys.h"
#include "HsmAPI.h"


int main(int argc,char *argv[])
{
  char     *hsmAddr = NULL;
  int       hsmPort = 0;
  char      hsmResponse[3];
  char      keyIndex[6];
  int       rv;

  hsmAddr = (char *)getenv("HSMSOFT_PORT");
  if ( hsmAddr == NULL )
  {
    printf("请设置加密机端口<HSMSOFT_PORT><HSMSOFT_ADDR>.\n");
    return -1;
  }
  hsmPort = strtol( hsmAddr, NULL, 10 );

  hsmAddr = (char *)getenv("HSMSOFT_ADDR");
  if ( hsmAddr == NULL )
  {
    printf("请设置软加密机地址<HSMSOFT_ADDR>.\n");
    return -1;
  }

  HsmSetServer(hsmAddr,hsmPort);
  memset(hsmResponse,0,sizeof(hsmResponse));

  if (argc < 3)
  {
    printf("Usage: HsmCtrl {GetRsaMasterKey|GetMasterKey|AddMasterKey|GetWorkKey|GetUserPin} {index} {keydata}\n");
    return -1;
  }
  memset(keyIndex,0,sizeof(keyIndex));
  strncpy(keyIndex,argv[2],5);


  rv = HsmOpen();
  if ( rv )
  {
    printf("连接加密机<%s:%d>失败\n",hsmAddr,hsmPort);
    return rv;
  }
  if (memcmp(argv[1],"GetMasterKey",12) == 0)
  {
    char   MainKeyA[33];
    char   MainKeyB[33];
    char   CheckValA[9];
    char   CheckValB[9];
    char   CheckVal[9];
    
    memset(MainKeyA,0,sizeof(MainKeyA));
    memset(MainKeyB,0,sizeof(MainKeyB));
    memset(CheckValA,0,sizeof(CheckValA));
    memset(CheckValB,0,sizeof(CheckValB));
    memset(CheckVal,0,sizeof(CheckVal));

    rv = HsmGetMainKey(keyIndex,MainKeyA,CheckValA,MainKeyB,CheckValB,CheckVal,hsmResponse);
    if ( rv )
    {
      printf("获取主密钥失败<%s>\n",hsmResponse);
      HsmClose();
      return rv;
    }
    printf("\n");
    printf("主密钥成分A: %32s\n",MainKeyA);
    printf("CheckValueA: %8s\n",CheckValA);
    printf("主密钥成分b: %32s\n",MainKeyB);
    printf("CheckValueb: %8s\n",CheckValB);
    printf("主密钥 CHK : %8s\n",CheckVal);
    printf("\n");
  }
  if (memcmp(argv[1],"GetRsaMasterKey",15) == 0)
  {
    char   RsaMainKey[257];

    
    memset(RsaMainKey,0,sizeof(RsaMainKey));


    rv = HsmGetRsaMainKey(keyIndex,RsaMainKey,hsmResponse);
    if ( rv )
    {
      printf("获取主密钥失败<%s>\n",hsmResponse);
      HsmClose();
      return rv;
    }
    printf("\n");
    printf("RSA[1]: %64s\n",RsaMainKey);
    printf("RSA[2]: %64s\n",RsaMainKey+64);
    printf("RSA[3]: %64s\n",RsaMainKey+128);
    printf("RSA[4]: %64s\n",RsaMainKey+192);
    printf("\n");
  }
  else if (memcmp(argv[1],"AddMasterKey",12) == 0)
  {
    char   CheckValA[9];
    
    memset(CheckValA,0,sizeof(CheckValA));
    
    if ( argc != 4 || strlen(argv[3]) != 32 )
    {
      printf("Usage: HsmCtrl AddMasterKey {index} {keydata}\n");
      HsmClose();
      return -1;
    }
    rv = HsmAddMainKey(keyIndex,argv[3],CheckValA,hsmResponse);
    if ( rv )
    {
      printf("获取主密钥失败<%s>\n",hsmResponse);
      HsmClose();
      return rv;
    }
    printf("\n");
    printf("主密钥: %32s\n",argv[3]);
    printf("CheckValueA: %8s\n",CheckValA);
  }
  else if ( memcmp(argv[1],"GetWorkKey",10) == 0 )
  {
    char   PinKey[33];
    char   MacKey[33];
    char   CheckValP[9];
    char   CheckValM[9];
    
    memset(PinKey,0,sizeof(PinKey));
    memset(MacKey,0,sizeof(MacKey));
    memset(CheckValP,0,sizeof(CheckValP));
    memset(CheckValM,0,sizeof(CheckValM));

    rv = HsmGetWorkerKey(keyIndex,PinKey,CheckValP,MacKey,CheckValM,hsmResponse);
    if ( rv )
    {
      printf("获取工作密钥失败<%s>\n",hsmResponse);
      HsmClose();
      return rv;
    }
    printf("\n");
    printf("PIN工作密钥: %32s\n",PinKey);
    printf("CheckValueP: %8s\n",CheckValP);
    printf("MAC工作密钥: %32s\n",MacKey);
    printf("CheckValueb: %8s\n",CheckValM);
    printf("\n");
  }
  else if ( memcmp(argv[1],"GetUserPin",10) == 0 )
  {
    char   keyIndexB[6];
    char   userId[33];
    char   pinData[257];
    char   newPinData[65];

    if ( argc != 6 || strlen(argv[5]) != 256 )
    {
      printf("Usage: HsmCtrl GetUserPin {indexA} {indexB} {userId} {pinData} \n");
      HsmClose();
      return -1;
    }
    
    memset(keyIndexB,0,sizeof(keyIndexB));
    memset(userId,0,sizeof(userId));
    memset(pinData,0,sizeof(pinData));
    memset(newPinData,0,sizeof(newPinData));

    strncpy(keyIndexB, argv[3], 5);
    strncpy(userId, argv[4], 30);
    strncpy(pinData, argv[5], 256);
    
    rv = HsmEnUserPIN(keyIndex, userId, pinData, keyIndexB, newPinData, hsmResponse);
    if ( rv )
    {
      printf("转加密用户密码失败<%s>\n",hsmResponse);
      HsmClose();
      return rv;
    }
    printf("\n");
    printf("NEW PIN DATA: %64s\n",newPinData);
    printf("\n");
  }
  HsmClose();
  return 0;
}


