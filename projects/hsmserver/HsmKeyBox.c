#include "ttsys.h"
#include "dbutil.h"
#include "DesUtil.h"
#include "DB_HsmKeyBox.h"
#include "HsmKeyBox.h"


static int   HsmKeyDataInit = 0;
//static int   GDebug = 0;
static char  GDatabaseUser[33];
static char  GDatabasePwsd[33];
static char  GDatabaseName[33];

//int HsmKeyBoxSetDebug(int debug)
//{
//  ELOG(ERROR,"DEBUG:  ADDR %08X",GDatabaseUser);
//  GDebug = debug;
//  return 0;
//}

int HsmKeyBoxSetDatabase(char *dbName,char *dbUser,char *dbPasswd)
{
  memset(GDatabaseUser,0,sizeof(GDatabaseUser));
  memset(GDatabasePwsd,0,sizeof(GDatabasePwsd));
  memset(GDatabaseName,0,sizeof(GDatabaseName));

  if ( dbName != NULL )
  {
    strncpy(GDatabaseName,dbName,32);
  }

  if ( dbUser != NULL )
  {
    strncpy(GDatabaseUser,dbUser,32);
  }

  if ( dbPasswd != NULL )
  {
    strncpy(GDatabasePwsd,dbPasswd,32);
  }

  return 0;
}

static DesKeyS GKeyS;

static char GMasterKey[33] = "A1A9B2B8C3C7D4D6E5F0E1D2C3B4A5FF\x00";

static DesKeyInit = 0;

static int DesKeysInit()
{
  char  key[17];

  memset(key,0x01,sizeof(key));
  
  endesmBlock(key,DES_MODE_2,GMasterKey,16);
  
  desmSetKey(key,DES_MODE_2,&GKeyS);
  
  DesKeyInit = 1;
  
  return 0;
}


static THsmKeyVector *GHsmKeyVector = NULL;

int HsmKeyBoxSetVector(THsmKeyVector *HsmKeyVector)
{
   GHsmKeyVector = HsmKeyVector;
   return 0;
}

int HsmKeyBoxLoadKey()
{
  HsmKeyBoxEx   hsmKeyBox ;
  int           rv;
  Select_Info   sInfo;
  char          binDataA[129],binDataB[129];
  char          md5Input[1024];
  char          md5Buf[33];
  THsmKeyItem  *hsmKeyItem;
  int           n = 0;

  if( DesKeyInit == 0 )
  {
    DesKeysInit();
  }
  
  rv = dbOpen( GDatabaseUser,GDatabasePwsd,GDatabaseName);
  if(rv)
  {
    ELOG(ERROR,"���ʼ������ݿ�<�û�:%s>ʧ��,���ݿ������:%d",GDatabaseUser,rv);
    return rv;
  }

  rv = DB_HsmKeyBoxEx_open_select(&sInfo);
  if ( rv )
  {
    ELOG(ERROR,"���ʼ������ݿ�<��:HsmKeyBoxEx>ʧ��,���ݿ������:%d",rv);
    dbClose();
    return rv;
  }

  for(;;)
  {
    if ( GHsmKeyVector->KeyCount - 1 >= HSM_KEY_MAX )
    {
      ELOG(ERROR,"WARN: ��Կ�ڴ�����,�޷���Ÿ�����Կ!!!");
      DB_HsmKeyBoxEx_close_select(&sInfo);
      dbClose();
      return rv;
    }

    memset(&hsmKeyBox,0,sizeof(HsmKeyBoxEx));
    memset(binDataA,0,sizeof(binDataA));
    memset(binDataB,0,sizeof(binDataB));
    
    memset(md5Input,0,sizeof(md5Input));
    memset(md5Buf,0,sizeof(md5Buf));

    rv = DB_HsmKeyBoxEx_fetch_select(&sInfo,&hsmKeyBox);
    if (rv == SQLNOTFOUND)
    {
      ELOG(ERROR,"INFO: ����ϵͳ������%05d����Կ",n);
      DB_HsmKeyBoxEx_close_select(&sInfo);
      dbClose();
      return 0;
    }
    else if ( rv )
    {
      ELOG(ERROR,"ERR: ���ʼ������ݿ�<��:HsmKeyBoxEx>ʧ��,���ݿ������:%d",rv);
      DB_HsmKeyBoxEx_close_select(&sInfo);
      dbClose();
      return rv;
    }
    
    if ( hsmKeyBox.keyIndex > HSM_KEY_MAX )
    {
      ELOG(ERROR,"WRAN: ��Կ<%d>����̫��.",hsmKeyBox.keyIndex);
      continue;
    }
    
    hsmKeyItem = &(GHsmKeyVector->item[hsmKeyBox.keyIndex]);

    sprintf(md5Input,"F1:%05d,F2:%1s,F3:%1s,F4:%03d,F5:%0256s,F6:%0256s",
                      hsmKeyBox.keyIndex, hsmKeyBox.keyType,
                      hsmKeyBox.keyFlag,  hsmKeyBox.keyLen,
                      hsmKeyBox.keyDataA, hsmKeyBox.keyDataB);

    MDString1(md5Input,md5Buf);
    if ( strncmp(md5Buf,hsmKeyBox.keyDac,32) != 0 )
    {
      ELOG(ERROR,"WRAN: ��Կ<%d>���ݱ��޸�,�Ƿ�.",hsmKeyBox.keyIndex);
      hsmKeyItem->keyStat = 'E';
      continue;
    }

    strToBin(hsmKeyBox.keyDataA,binDataA,hsmKeyBox.keyLen);
    strToBin(hsmKeyBox.keyDataB,binDataB,hsmKeyBox.keyLen);

    dedesmBlockKeyS(&GKeyS,binDataA,hsmKeyBox.keyLen);
    dedesmBlockKeyS(&GKeyS,binDataB,hsmKeyBox.keyLen);
    
    hsmKeyItem->keyIdx  = hsmKeyBox.keyIndex;
    hsmKeyItem->keyType = hsmKeyBox.keyType[0];
    hsmKeyItem->keyFlag = hsmKeyBox.keyFlag[0];
    hsmKeyItem->keyLen  = hsmKeyBox.keyLen;
    memcpy(hsmKeyItem->keyDataA,binDataA,hsmKeyBox.keyLen);
    memcpy(hsmKeyItem->keyDataB,binDataB,hsmKeyBox.keyLen);
    hsmKeyItem->keyStat = 'Y';
    
    GHsmKeyVector->KeyCount++;
    n++;

//    if ( GDebug )
//    {
//      memset(hexData,0,sizeof(hexData));
//      binToStr(binData,hexData,16);
//      ELOG(ERROR,"DEBUG: KEYIDX %d,%s",hsmKeyBox.keyIndex,hexData);
//    }

  }

  return 0;
}


int HsmKeyBoxUpdateKey(THsmKeyInfo  *hsmKeyInfo)
{
  HsmKeyBoxEx   hsmKeyBox ;
  int           rv;
  char          newkey[129];
  char          md5Input[1024];
  char          md5Buf[33];
  THsmKeyItem  *hsmKeyItem;
  int           flag ;

  if ( GHsmKeyVector->KeyCount - 1 > HSM_KEY_MAX || hsmKeyInfo->keyIdx <= 0 || hsmKeyInfo->keyIdx - 1 > HSM_KEY_MAX )
  {
     ELOG(INFO,"��Կ<%05d>��ŷǷ�",hsmKeyInfo->keyIdx);
     return TTS_NOTFOUND;
  }

  if ( hsmKeyInfo->keyType != '1' && hsmKeyInfo->keyType != '2' && hsmKeyInfo->keyType != '3') 
  {
     ELOG(INFO,"��Կ<%05d>��Կ���ͷǷ�",hsmKeyInfo->keyIdx);
     return TTS_NOTFOUND;
  }

  if ( hsmKeyInfo->keyFlag != '1' && hsmKeyInfo->keyFlag != '2' )
  {
     ELOG(INFO,"��Կ<%05d>��Կ���ͷǷ�",hsmKeyInfo->keyIdx);
     return TTS_NOTFOUND;
  }

  if ( hsmKeyInfo->keyLen % 16 )
  {
     ELOG(INFO,"��Կ<%05d>��Կ���ȷǷ�",hsmKeyInfo->keyIdx);
     return TTS_NOTFOUND;
  }
  
  if( DesKeyInit == 0 )
  {
    DesKeysInit();
  }

  hsmKeyItem = &(GHsmKeyVector->item[hsmKeyInfo->keyIdx]);

  if ( hsmKeyItem->keyFlag == '1' )
  {
     ELOG(INFO,"��Կ<%05d>���ɸ���",hsmKeyInfo->keyIdx);
     return TTS_NOTFOUND;
  }
  
  if ( hsmKeyInfo->keyStat == 'L' && hsmKeyInfo->keyFlag == '1')
  {
    hsmKeyItem->keyFlag = '1'; 
  }

  if ( hsmKeyItem->keyStat == 'Y' || hsmKeyItem->keyStat == 'E' )
  {
    ELOG(ERROR,"INFO: ����ϵͳ��ʼ������Կ<%d>",hsmKeyInfo->keyIdx);
    flag = 1;
  }
  else
  {
    ELOG(ERROR,"INFO: ����ϵͳ��ʼ������Կ<%d>",hsmKeyInfo->keyIdx);
    flag = 0;
  }

  memset(&hsmKeyBox,0,sizeof(HsmKeyBoxEx));

  hsmKeyBox.keyIndex = hsmKeyInfo->keyIdx;
  hsmKeyBox.keyType[0]  = hsmKeyInfo->keyType;
  hsmKeyBox.keyFlag[0]  = hsmKeyInfo->keyFlag;
  hsmKeyBox.keyLen   = hsmKeyInfo->keyLen;

  memset(newkey,0,sizeof(newkey));
  memcpy(newkey,hsmKeyInfo->keyDataA,hsmKeyInfo->keyLen);

  endesmBlockKeyS(&GKeyS,newkey,hsmKeyInfo->keyLen);
  binToStr(newkey,hsmKeyBox.keyDataA,hsmKeyInfo->keyLen);

  memset(newkey,0,sizeof(newkey));
  memcpy(newkey,hsmKeyInfo->keyDataB,hsmKeyInfo->keyLen);

  endesmBlockKeyS(&GKeyS,newkey,hsmKeyInfo->keyLen);
  binToStr(newkey,hsmKeyBox.keyDataB,hsmKeyInfo->keyLen);

  memset(md5Input,0,sizeof(md5Input));
  memset(md5Buf,0,sizeof(md5Buf));

  sprintf(md5Input,"F1:%05d,F2:%1s,F3:%1s,F4:%03d,F5:%0256s,F6:%0256s",
                      hsmKeyBox.keyIndex, hsmKeyBox.keyType,
                      hsmKeyBox.keyFlag,  hsmKeyBox.keyLen,
                      hsmKeyBox.keyDataA, hsmKeyBox.keyDataB);

  MDString1(md5Input,md5Buf);
  strncpy(hsmKeyBox.keyDac,md5Buf,32);

  rv = dbOpen( GDatabaseUser,GDatabasePwsd,GDatabaseName);
  if(rv)
  {
    ELOG(ERROR,"�򿪼���ϵͳ���ݿ�<�û�:%s>ʧ��,���ݿ������:%d",GDatabaseUser,rv);
    return rv;
  }

  if ( flag )
  {
    rv = DB_HsmKeyBoxEx_update_by_keyIndex(hsmKeyBox.keyIndex,&hsmKeyBox);
  }
  else
  {
    rv = DB_HsmKeyBoxEx_add(&hsmKeyBox);
  }
  if ( rv )
  {
    ELOG(ERROR,"��Կϵͳ%s��Կ<%d>ʧ��,���ݿ������:%d",(flag == 0 ? "����" : "����" ),hsmKeyBox.keyIndex,rv);
    dbRollback();
    dbClose();
    return rv;
  }

  dbCommit();
  dbClose();

  hsmKeyItem->keyIdx  = hsmKeyBox.keyIndex;
  hsmKeyItem->keyType = hsmKeyBox.keyType[0];
  hsmKeyItem->keyFlag = hsmKeyBox.keyFlag[0];
  hsmKeyItem->keyLen  = hsmKeyBox.keyLen;
  memcpy(hsmKeyItem->keyDataA,hsmKeyInfo->keyDataA,hsmKeyBox.keyLen);
  memcpy(hsmKeyItem->keyDataB,hsmKeyInfo->keyDataB,hsmKeyBox.keyLen);
  hsmKeyItem->keyStat = 'Y';

  if ( !flag )
  {
    GHsmKeyVector->KeyCount++;
  }

  ELOG(INFO,"INFO: ��Կϵͳ%s��Կ<%d>�ɹ�",(flag == 0 ? "����" : "����" ),hsmKeyItem->keyIdx);
  return 0;
}

int HsmKeyBoxGetKey(THsmKeyInfo  *hsmKeyInfo)
{
  int       rv;

  if ( GHsmKeyVector == NULL || hsmKeyInfo->keyIdx - 1 > HSM_KEY_MAX )
  {
    return TTS_ENULL;
  }
  
  if ( GHsmKeyVector->item[hsmKeyInfo->keyIdx].keyStat != 'Y' )
  {
    ELOG(ERROR,"WARN: ��Կϵͳδ�ҵ���Կ<%d>",hsmKeyInfo->keyIdx);
    return TTS_NOTFOUND;
  }

  memcpy(hsmKeyInfo, &(GHsmKeyVector->item[hsmKeyInfo->keyIdx]), sizeof(THsmKeyInfo) );
 
  return TTS_SUCCESS;
}

