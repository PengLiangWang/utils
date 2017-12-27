#include "ttsys.h"
#include "HsmAPI.h"


int main(int argc,char *argv[])
{
  char     *hsmHost = NULL;
  char      keyIndex[6];
  int       rv;

  hsmHost = (char *)getenv("HSMSOFT_HOST");
  if ( hsmHost == NULL )
  {
    printf("请设置加密机端口<HSMSOFT_HOST> xxx.xxx.xxx.xxx:8898 .\n");
    return -1;
  }
  
  Hsm2SetServer(hsmHost);

  if (argc < 3)
  {
    printf("Usage: HsmCtrl2 {GetRsaMasterKey|GetMasterKey|AddMasterKey|GetWorkKey|GetUserPin|LockKey|GetRsaPubKey} {index} {keydata}\n");
    return -1;
  }
  memset(keyIndex,0,sizeof(keyIndex));
  strncpy(keyIndex,argv[2],5);



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

    rv = Hsm2GetMainKey(keyIndex,MainKeyA,CheckValA,MainKeyB,CheckValB,CheckVal);
    if ( rv )
    {
      printf("获取主密钥失败<%d>\n", rv);
      //Hsm2Close();
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
    char   RsaItem[65];
    
    memset(RsaMainKey,0,sizeof(RsaMainKey));


    rv = Hsm2GetRsaMainKey(keyIndex,RsaMainKey);
    if ( rv )
    {
      printf("获取主密钥失败<%d>\n", rv);
      return rv;
    }
    printf("\n");
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey, 64);
    printf("RSA[1]: %64s\n",RsaItem);
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey+64, 64);
    printf("RSA[2]: %64s\n",RsaItem);
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey+128, 64);
    printf("RSA[3]: %64s\n",RsaItem);
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey+192, 64);
    printf("RSA[4]: %64s\n",RsaItem);
    printf("\n");
  }
  if (memcmp(argv[1],"GetRsaPubKey",12) == 0)
  {
    char   RsaMainKey[257];
    char   RsaItem[65];
    
    memset(RsaMainKey,0,sizeof(RsaMainKey));


    rv = Hsm2GetRsaPubKey(keyIndex,RsaMainKey);
    if ( rv )
    {
      printf("获取主密钥失败<%d>\n", rv);
      return rv;
    }
    printf("\n");
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey, 64);
    printf("RSA[1]: %64s\n",RsaItem);
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey+64, 64);
    printf("RSA[2]: %64s\n",RsaItem);
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey+128, 64);
    printf("RSA[3]: %64s\n",RsaItem);
    memset(RsaItem, 0, sizeof(RsaItem));
    memcpy(RsaItem, RsaMainKey+192, 64);
    printf("RSA[4]: %64s\n",RsaItem);
    printf("\n");
  }
  else if (memcmp(argv[1],"AddMasterKey",12) == 0)
  {
    char   CheckValA[9];
    
    memset(CheckValA,0,sizeof(CheckValA));
    
    if ( argc != 4 || strlen(argv[3]) != 32 )
    {
      printf("Usage: HsmCtrl2 AddMasterKey {index} {keydata}\n");
      //Hsm2Close();
      return -1;
    }
    rv = Hsm2AddMainKey(keyIndex,argv[3],CheckValA);
    if ( rv )
    {
      printf("获取主密钥失败<%d>\n", rv);
      //Hsm2Close();
      return rv;
    }
    printf("\n");
    printf("主密钥: %32s\n",argv[3]);
    printf("CheckValueA: %8s\n",CheckValA);
  }
  else if (memcmp(argv[1],"LockKey",7) == 0)
  {
    rv = Hsm2LockMainKey(keyIndex);
    if ( rv )
    {
      printf("获取主密钥失败<%d>\n", rv);
      return rv;
    }
    printf("\n");
    printf("锁定主密钥(%s)成功\n", keyIndex);
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

    rv = Hsm2GetWorkerKey(keyIndex,PinKey,CheckValP,MacKey,CheckValM);
    if ( rv )
    {
      printf("获取工作密钥失败<%d>\n", rv);
      //Hsm2Close();
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
      printf("Usage: HsmCtrl2 GetUserPin {indexA} {indexB} {userId} {pinData} \n");
      //Hsm2Close();
      return -1;
    }
    
    memset(keyIndexB,0,sizeof(keyIndexB));
    memset(userId,0,sizeof(userId));
    memset(pinData,0,sizeof(pinData));
    memset(newPinData,0,sizeof(newPinData));

    strncpy(keyIndexB, argv[3], 5);
    strncpy(userId, argv[4], 30);
    strncpy(pinData, argv[5], 256);
    
    rv = Hsm2GetUserPIN(keyIndex, userId, pinData, keyIndexB, newPinData);
    if ( rv )
    {
      printf("转加密用户密码失败<%d>\n", rv);
      //Hsm2Close();
      return rv;
    }
    printf("\n");
    printf("NEW PIN DATA: %64s\n",newPinData);
    printf("\n");
  }
  return 0;
}


