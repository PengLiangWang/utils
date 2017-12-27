#include "ttsys.h"
static  char      GSendBuffer[2048];
static  char      GRecvBuffer[2048];  

static  char      GHsmHost[16];      
static  int       GHsmPort = 0;     

static  char      GErrText[256];   
static  int       GErrCode;       

static  char     *GInData = NULL;   
static  char     *GOutData = NULL; 
static  TSocket  *GHsmSocket = NULL;

static  int       GHsmOpend = 0;  
static  int       GHsmSetServer = 0;

int HsmSetServer(char *hsmHost,int hsmPort)
{
  memset(GHsmHost,0,sizeof(GHsmHost));
  strncpy(GHsmHost,hsmHost,15);
  GHsmPort = hsmPort;
  return 0;
}

int HsmSetServer2(char *hsmHost)
{
   if ( GHsmOpend || GHsmSetServer)
   {
      return 0;
   }
   {
      int   len = 0;
      char *ptr;

      ptr = strchr(hsmHost , ':' );
      if ( ptr != NULL )
      {
         len = ptr - hsmHost;
         strncpy( GHsmHost , hsmHost , len ) ;
         GHsmPort = strtol(ptr + 1,NULL,10);
         return TTS_SUCCESS;
     }
     else
     {
       return TTS_EINVAL;
     }
   }
}

static int HsmComm(int sndLen,int *rcvLen)
{
  int        rv;

  if (sndLen < 2 )
  {
    return TTS_EINVAL;
  }

  rv = SocketWriteEx(GHsmSocket,GSendBuffer,sndLen,10,SOCKET_HEX_LENGTH);
  if ( rv )
  {
    return rv;
  }
  
  rv = SocketReadEx(GHsmSocket,GRecvBuffer,rcvLen,10,SOCKET_HEX_LENGTH);
  if ( rv )
  {
    return rv;
  }
  
  GOutData = GRecvBuffer + 4;

  return 0;
}

int HsmOpen()
{
   if ( GHsmOpend )
   {
       return 0;
   }

   {
      int rv = SocketConnect(&GHsmSocket,GHsmHost,GHsmPort);
      //#define  HSM_CONNECT_TIMEOUT  5
      //int rv = SocketConnectEx(&GHsmSocket, GHsmHost, GHsmPort, HSM_CONNECT_TIMEOUT);
      if ( rv != TTS_SUCCESS )
      {
        ELOG(ERROR,"连接加密机(%s:%d)失败,(%s:%d)", GHsmHost, GHsmPort, GetStrError(rv), rv);
        return rv;
      }

      GHsmOpend  = 1;
   }

   return TTS_SUCCESS;
}

int HsmClose()
{
  
  if ( GHsmOpend == 0 )
  {
    return 0;
  }

  SocketClose(&GHsmSocket);
  
  GHsmSocket = NULL;

  GHsmOpend = 0;

  return TTS_SUCCESS;
}

static int HsmDataInit()
{
  memset(GSendBuffer,0,sizeof(GSendBuffer));
  memset(GRecvBuffer,0,sizeof(GRecvBuffer));
  memset(GErrText,0,sizeof(GErrText));
  GErrCode = 0;

  GInData  = GSendBuffer;
  GOutData = GRecvBuffer;

  return 0;
}

int HsmAddMainKey(char * index,char *MainKey,char *checkVal,char *hsmResp)
{
  int      sndLen = 0 ,rcvLen = 0;
  int      rv;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();
  
  sprintf(GInData,"01%05s",index);
  sndLen += 7;
  
  strncpy(GInData + sndLen,MainKey,32);
  sndLen += 32;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(checkVal,GOutData + 2,8);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }

}

int HsmGetMainKey(char *index,char *MainKeyA,char *CheckValA,char *MainKeyB,char *CheckValB,char *CheckVal,char *hsmResp)
{
  int      sndLen = 0 ,rcvLen = 0;
  int      rv;


  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"02%05s",index);
  sndLen += 7;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0 )
  {
     strncpy(MainKeyA,GOutData + 2, 32);
     strncpy(CheckValA,GOutData + 34, 8);
     strncpy(MainKeyB,GOutData + 42, 32);
     strncpy(CheckValB,GOutData +74, 8);
     strncpy(CheckVal,GOutData +82, 8);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}


int HsmGetRsaMainKey(char *index,char *RsaKey,char *hsmResp)
{
  int      sndLen = 0 ,rcvLen = 0;
  int      rv;


  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"03%05s",index);
  sndLen += 7;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0 )
  {
     strncpy(RsaKey,GOutData + 2, 256);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}


int HsmGetWorkerKey(char *index,char *PinKey,char *PinCheckValue,char *Mackey,char *MacCheckValue,char *hsmResp)
{
  int       sndLen = 0 ,rcvLen = 0;
  int       rv;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"13%05s",index);
  sndLen += 7;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(PinKey,GOutData + 2, 32);
     strncpy(PinCheckValue,GOutData + 34, 8);
     strncpy(Mackey,GOutData + 42, 32);
     strncpy(MacCheckValue,GOutData +74, 8);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}

int HsmCalcMAC(char *index,char *MacKey,char *data,int dataLen,char *MAC,char *hsmResp)
{
  int       sndLen = 0 ,rcvLen = 0;
  int       rv;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"22%05s",index);
  sndLen += 7;

  memcpy(GInData + sndLen,MacKey,32);
  sndLen += 32;

  sprintf(GInData + sndLen,"%03d",dataLen);
  sndLen += 3;

  binToStr(data,GInData + sndLen,dataLen);
  sndLen += dataLen * 2;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(MAC,GOutData + 2, 8);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}

int HsmCalcPIN(char *indexA,char *PinKey,char *indexB,char *workKey,char *pinData,char *AcctNo,char *newPinData,char *hsmResp)
{
  int       sndLen = 0 ,rcvLen = 0;
  int       rv;
  char      PAN[17];
  int       i;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"21%05s",indexA);
  sndLen += 7;

  memcpy(GInData + sndLen,PinKey,32);
  sndLen += 32;

  sprintf(GInData + sndLen,"%05s",indexB);
  sndLen += 5;

  sprintf(GInData + sndLen,"%032s",workKey);
  sndLen += 32;

  sprintf(GInData + sndLen,"%016s",pinData);
  sndLen += 16;

  memset(PAN,0,sizeof(PAN));
  if( strlen(AcctNo) < 16 )
  {
    for (i = 0; i < 16 - strlen(AcctNo) ; i++)
    {
      PAN[i] = '0';
    }
    strcat(PAN,AcctNo);
    PAN[16] = 0;
  }
  else
  {
    memcpy(PAN ,AcctNo + strlen(AcctNo) - 16,16);
  }

  sprintf(GInData + sndLen,"%016s",PAN);
  sndLen += 16;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(newPinData,GOutData + 2, 16);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}

int HsmEnPIN(char *indexA,char *PinKey,char *pinData,char *AcctNo,char *newPinData,char *hsmResp)
{
  int       sndLen = 0 ,rcvLen = 0;
  int       rv;
  char      PAN[17];
  int       i;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"26%05s",indexA);
  sndLen += 7;

  memcpy(GInData + sndLen,PinKey,32);
  sndLen += 32;

  sprintf(GInData + sndLen,"%016s",pinData);
  sndLen += 16;

  memset(PAN,0,sizeof(PAN));
  if ( strlen(AcctNo) < 16 )
  {
    for (i = 0; i < 16 - strlen(AcctNo) ; i++)
    {
      PAN[i] = '0';
    }
    strcat(PAN,AcctNo);
    PAN[16] = 0;
  }
  else
  {
    memcpy(PAN ,AcctNo + strlen(AcctNo) - 16,16);
  }

  sprintf(GInData + sndLen,"%016s",PAN);
  sndLen += 16;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(newPinData,GOutData + 2, 16);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}

int HsmCalcPIN3DES(char *indexA,char *PinKey,char *indexB,char *factorA,char *factorB,char *factorC,char *pinData,char *AcctNo,char *newPinData,char *hsmResp)
{
  int       sndLen = 0 ,rcvLen = 0;
  int       rv;
  char      PAN[17];
  int       i;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"24%05s",indexA);
  sndLen += 7;

  memcpy(GInData + sndLen,PinKey,32);
  sndLen += 32;

  sprintf(GInData + sndLen,"%05s3",indexB);
  sndLen += 6;

  sprintf(GInData + sndLen,"%016s",factorA);
  sndLen += 16;

  sprintf(GInData + sndLen,"%016s",factorB);
  sndLen += 16;

  sprintf(GInData + sndLen,"%016s",factorC);
  sndLen += 16;

  sprintf(GInData + sndLen,"%016s",pinData);
  sndLen += 16;

  memset(PAN,0,sizeof(PAN));
  if ( strlen(AcctNo) < 16 )
  {
    for (i = 0; i < 16 - strlen(AcctNo) ; i++)
    {
      PAN[i] = '0';
    }
    strcat(PAN,AcctNo);
    PAN[16] = 0;
  }
  else
  {
    memcpy(PAN ,AcctNo + strlen(AcctNo) - 16,16);
  }

  sprintf(GInData + sndLen,"%016s",PAN);
  sndLen += 16;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(newPinData,GOutData + 2, 16);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}

int FormatHsmHostString(char *hsmHost,char *hsmAddr,int *hsmPort)
{
  int   len = 0;
  char *ptr;

  ptr = strchr(hsmHost , ':' );
  if ( ptr != NULL )
  {
    len = ptr - hsmHost;
    strncpy( hsmAddr , hsmHost , len ) ;
    *hsmPort = strtol(ptr + 1,NULL,10) ;
    return TTS_SUCCESS;
  }
  else
  {
    return TTS_EINVAL;
  }
}

int FormatAccountFromTrack2(char *track2,char *acct)
{
  char     *ptrA,*ptrB,*ptr = NULL ;
  int      offset = 0 ,len = 0 ;


 ptrA = strchr(track2 , '=' );
 if ( ptrA == NULL )
 {
   ptrB = strchr(track2 , 'D' );
   if ( ptrB == NULL )
   {
     len = 19;
     strncpy( acct , track2 , strlen(track2) > 19 ? 19 : strlen(track2) ) ;
   }
   else
   {
     ptr = ptrB;
   }
 }
 else
 {
   ptr = ptrA;
 }

 if ( ptr != NULL)
 {
   len = ptr - track2;
   if ( len > 19 )
   {
     offset = len - 19 ;
     len = 19;
   }
   strncpy( acct , track2 + offset, len ) ;
 }

 return 0 ;
}



//转加密用户密码
int HsmEnUserPIN(char *indexA,char *userId,char *pinData, char *indexB, char *newPinData,char *hsmResp)
{
  int       sndLen = 0, rcvLen = 0, dataLen = 0;
  int       rv;
  char      PAN[17];
  int       i;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"A1%05s",indexA);
  sndLen += 7;

  dataLen = strlen(userId);
  sprintf(GInData + sndLen, "%02d",dataLen);
  sndLen += 2;

  memcpy(GInData + sndLen, userId, dataLen);
  sndLen += dataLen;

  dataLen = strlen(pinData);
  sprintf(GInData + sndLen,"%03d",dataLen/2);
  sndLen += 3;
  memcpy(GInData + sndLen, pinData, dataLen);
  sndLen += dataLen;

  sprintf(GInData + sndLen,"%05s",indexB);
  sndLen += 5;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(newPinData,GOutData + 2, 64);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}


//转加密用户密码
int HsmCalcPinRsa(char *indexA,char *panData,char *pinData, char *indexB, char *fA, char *fB, char *fC, char *newPinData,char *hsmResp)
{
  int       sndLen = 0, rcvLen = 0, dataLen = 0;
  int       rv;
  char      PAN[17];
  int       i;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"B1%05s",indexA);
  sndLen += 7;

  sprintf(GInData + sndLen, "%016s",panData);
  sndLen += 16;

  dataLen = strlen(pinData);
  sprintf(GInData + sndLen,"%03d",dataLen/2);
  sndLen += 3;
  memcpy(GInData + sndLen, pinData, dataLen);
  sndLen += dataLen;

  sprintf(GInData + sndLen,"%05s",indexB);
  sndLen += 5;

  sprintf(GInData + sndLen,"%016s",fA);
  sndLen += 16;

  sprintf(GInData + sndLen,"%016s",fB);
  sndLen += 16;

  ssprintf(GInData + sndLen,"%016s",fC);
  sndLen += 16;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(newPinData,GOutData + 2, 16);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}

//转加密用户密码
int HsmCalcPinRsa2(char *indexA,char *panData,char *pinData, char *workKey, char *pinType, char *indexB, char *newPinData,char *hsmResp)
{
  int       sndLen = 0, rcvLen = 0, dataLen = 0;
  int       rv;
  char      PAN[17];
  int       i;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"B2%05s",indexA);
  sndLen += 7;

  dataLen = strlen(panData);
  sprintf(GInData + sndLen, "%02d", dataLen);
  sndLen += 2;
  memcpy(GInData + sndLen,panData, dataLen);
  sndLen += dataLen;

  dataLen = strlen(pinData);
  sprintf(GInData + sndLen,"%03d",dataLen/2);
  sndLen += 3;
  memcpy(GInData + sndLen, pinData, dataLen);
  sndLen += dataLen;

  dataLen = strlen(workKey);
  if(dataLen != 16 && dataLen != 32)
  {
    return TTS_EINVAL;
  }
  else if(dataLen == 16)
  {
    memcpy(GInData + sndLen, workKey, dataLen);
    sndLen += dataLen;
  }

  memcpy(GInData + sndLen, workKey, dataLen);
  sndLen += dataLen;

  if(strlen(pinType) != 1 || ( pinType[0] != '1' && pinType[0] != '2'))
  {
    return TTS_EINVAL;
  }

  memcpy(GInData + sndLen, pinType, 1);
  sndLen += 1;

  sprintf(GInData + sndLen,"%05s",indexB);
  sndLen += 5;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(newPinData,GOutData + 2, 16);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}

//转加密用户密码2
int HsmCalcPinRsa2(char *indexA, char *pinData, char *fA, char *fB, char *fC, char *newPinData,char *hsmResp)
{
  int       sndLen = 0, rcvLen = 0, dataLen = 0;
  int       rv;
  char      PAN[17];
  int       i;

  if ( GHsmOpend != 1 )
  {
    rv = HsmOpen();
    if ( rv )
    {
      return rv;
    }
  }

  HsmDataInit();

  sprintf(GInData,"B3%05s",indexA);
  sndLen += 7;

  dataLen = strlen(pinData);
  sprintf(GInData + sndLen,"%02d",dataLen);
  sndLen += 2;
  memcpy(GInData + sndLen, pinData, dataLen);
  sndLen += dataLen;

  sprintf(GInData + sndLen,"%016s",fA);
  sndLen += 16;

  sprintf(GInData + sndLen,"%016s",fB);
  sndLen += 16;

  ssprintf(GInData + sndLen,"%016s",fC);
  sndLen += 16;

  rv = HsmComm(sndLen,&rcvLen);
  if ( rv )
  {
    return rv;
  }

  if ( strncmp(GOutData,"00",2) == 0)
  {
     strncpy(newPinData,GOutData + 2, 16);
     return TTS_SUCCESS;
  }
  else
  {
    memcpy(hsmResp,GOutData,2);
    strncpy(GErrText,GOutData + 2,rcvLen - 4);
    GErrCode = TTS_EGENERAL;
    return TTS_EGENERAL;
  }
}
