
#include "HsmAPI2.h"
#include <sys/socket.h>
#include <stdarg.h>
#include <stdio.h>

static int      HsmConnected = 0;
static TSocket *HsmSocket = NULL;
static int      HsmSocketHandle = -1;
static char     HsmServerInfo[22]={0};
static int      OnExitEvent = 0;

static void HsmOnExit()
{
   if ( HsmConnected && HsmSocket )
   {
      ELOG(INFO, "(Event OnExit)关闭与系统加密机(%s)的连接", HsmServerInfo);
      SocketClose(&HsmSocket);
      HsmConnected = 0;
   }

   return ;
}

static int HsmConnect( )
{
   int   res;
   char  serverIP[17];
   int   serverPort;

   if( HsmConnected )
   {
      return 0;
   }
   else if ( HsmServerInfo[0] == 0 )
   {
      ELOG(ERROR, "函数参数serverInfo不正确.");
      return TTS_EINVAL;
   }

   memset(serverIP, 0, sizeof(serverIP));
   serverPort = 0;

   res = sscanf(HsmServerInfo, "%[^:]:%d", serverIP, &serverPort);
   if ( res != 2 )
   {
      ELOG(ERROR, "系统加密机主机信息不正确(%s)", HsmServerInfo);
      return TTS_EINVAL;
   }

   res = SocketConnect(&HsmSocket, serverIP, serverPort);
   if ( res )
   {
      ELOG(ERROR, "连接系统加密机(%s)失败,错误信息:%d,%s", HsmServerInfo, res, GetStrError(res));
      return res;
   }

   HsmSocketHandle = SocketGetHandle(HsmSocket);

   {
      struct timeval   timeout;

      timeout.tv_sec  = 10;
      timeout.tv_usec = 0;

      if ( setsockopt( HsmSocketHandle, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout) ) )
      {
         res = errno;
         ELOG(ERROR, "设置通讯参数TIMEOUT 失败,错误信息:%d,%s", res, GetStrError(res));
         SocketClose(&HsmSocket);
         return res;
      }

      HsmConnected = 1;
      ELOG(INFO, "与系统加密机(%s)成功对接", HsmServerInfo);

      if ( OnExitEvent == 0 )
      {
         atexit(HsmOnExit);
         OnExitEvent = 1;
      }
      return TTS_SUCCESS;
   }
}

static char  HsmSendBuf[4096];
static char  HsmRecvBuf[4096];
static char  HsmRespMsg[256];

static int HsmResetBuffer(char **sndBuf, char **rcvBuf)
{
   memset(HsmSendBuf, 0, sizeof(HsmSendBuf));
   memset(HsmRecvBuf, 0, sizeof(HsmRecvBuf));
   memset(HsmRespMsg, 0, sizeof(HsmRespMsg));

   *sndBuf = HsmSendBuf + 2;
   *rcvBuf = HsmRecvBuf + 2;

   return 0;
}

static int HsmCall(int sndLen, int *rcvLen)
{
   int   res;

   HsmSendBuf[0] = (int)((sndLen / 0xFF) & 0xFF);
   HsmSendBuf[1] = (int)((sndLen % 0xFF) & 0xFF);
   sndLen += 2;

   res = send(HsmSocketHandle, HsmSendBuf, sndLen, 0);
   if ( res < 0 )
   {
      res = errno;
      ELOG(ERROR, "与系统加密机通讯(send)失败,错误信息:%d,%s", res, GetStrError(res));
      SocketClose(&HsmSocket);
      HsmConnected = 0;

      /** 如果发送失败,重新连接一次并发送*/

      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "重新连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }

      res = send(HsmSocketHandle, HsmSendBuf, sndLen, 0);
      if ( res < 0 )
      {
         res = errno;
         ELOG(ERROR, "与系统加密机通讯(send)失败,错误信息:%d,%s", res, GetStrError(res));
         SocketClose(&HsmSocket);
         HsmConnected = 0;
         return res;
      }
   }

   res = recv(HsmSocketHandle, HsmRecvBuf, 2, 0);
   if ( res < 0 )
   {
      res = errno;
      ELOG(ERROR, "与系统加密机通讯(recv)失败,错误信息:%d,%s", res, GetStrError(res));
      SocketClose(&HsmSocket);
      HsmConnected = 0;
      return res;
   }

   *rcvLen = (int)((HsmRecvBuf[0] & 0xFF) * 0xFF ) + (int)(HsmRecvBuf[1] & 0xFF);
   if ( *rcvLen <= 0 )
   {
      ELOG(ERROR, "与系统加密机通讯(recv)失败,数据长度非法");
      SocketClose(&HsmSocket);
      HsmConnected = 0;
      return TTS_EINVAL;
   }

   res = recv(HsmSocketHandle, HsmRecvBuf + 2, *rcvLen, 0);
   if ( res < 0 )
   {
      res = errno;
      ELOG(ERROR, "与系统加密机通讯(recv)失败,错误信息:%d,%s", res, GetStrError(res));
      SocketClose(&HsmSocket);
      HsmConnected = 0;
      return res;
   }

   return TTS_SUCCESS;
}

#define  HDT_ASC  'A'
#define  HDT_LLL  'L'
#define  HDT_LLA  'B'
#define  HDT_LLB  'C'
#define  HDT_NULL 0

static int HsmProcess(char *sndBuf, char *rcvBuf, char *cmd,...)
{
   int        res;
   int        sndLen = 0, rcvLen = 0;
   va_list    args;
   int        valType;
   int        valLen;
   char      *valP;

   strncpy(sndBuf, cmd, 2);
   sndLen += 2;
   sndBuf += 2;

   va_start(args, cmd);

   for (; ; )
   {
      valType = va_arg(args, int);
      if ( valType == HDT_NULL )
      {
         break;
      }

      valP = va_arg(args, char *);
      valLen = va_arg(args, int);

      if ( valLen == 0 || valP == HDT_NULL )
      {
         continue;
      }

      switch ( valType )
      {
         case HDT_ASC:
            memcpy(sndBuf, valP, valLen);
            sndBuf += valLen;
            sndLen += valLen;
            break;
         case HDT_LLA:
            sprintf(sndBuf, "%02d", valLen);
            sndBuf += 2;
            sndLen += 2;
            memcpy(sndBuf, valP, valLen);
            sndLen += valLen;
            sndBuf += valLen;
            break;
         case HDT_LLB:
            sprintf(sndBuf, "%03d", valLen);
            sndBuf += 3;
            sndLen += 3;
            memcpy(sndBuf, valP, valLen);
            sndLen += valLen;
            sndBuf += valLen;
            break;
         case HDT_LLL:
            sprintf(sndBuf, "%03d", valLen / 2);
            sndBuf += 3;
            sndLen += 3;
            memcpy(sndBuf, valP, valLen);
            sndLen += valLen;
            sndBuf += valLen;
            break;
         default:
            ELOG(ERROR, "数据类型不符合定义(%c)", valType);
            va_end(args);
            return TTS_EINVAL;
      }
   }

   va_end(args);

   return HsmCall(sndLen, &rcvLen);
}

int Hsm2SetServer(char *serverInfo)
{
   char  serverIP[17];
   int   serverPort, res;

   if ( serverInfo && strcmp(HsmServerInfo, serverInfo) == 0 )
   {
      return 0;
   }

   memset(serverIP, 0, sizeof(serverIP));
   serverPort = 0;

   res = sscanf(serverInfo, "%[^:]:%d", serverIP, &serverPort);
   if ( res != 2 )
   {
      ELOG(ERROR, "系统加密机主机信息不正确(%s)", serverInfo);
      return TTS_EINVAL;
   }
   memset(HsmServerInfo, 0, sizeof(HsmServerInfo));
   strcpy(HsmServerInfo, serverInfo);

   return 0;
}

int Hsm2Close()
{
   if ( HsmSocket )
   {
      ELOG(INFO, "关闭与系统加密机(%s)的连接", HsmServerInfo);
      SocketClose(&HsmSocket);
   }

   HsmConnected = 0;

   return 0;
}


int Hsm2AddMainKey(char * index,char *keyData,char *checkVal)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "01",
                    HDT_ASC, index, 5,
                    HDT_ASC, keyData, 32,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      memcpy(checkVal, rcvBuf + 4, 8);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2LockMainKey(char * index)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "05",
                    HDT_ASC, index, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetMainKey(char *index,char *MainKeyA,char *CheckValA,char *MainKeyB,char *CheckValB,char *CheckVal)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "02",
                    HDT_ASC, index, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(MainKeyA, rcvBuf, 32);
      rcvBuf += 32;
      memcpy(CheckValA, rcvBuf, 8);
      rcvBuf += 8;
      memcpy(MainKeyB, rcvBuf, 32);
      rcvBuf += 32;
      memcpy(CheckValB, rcvBuf, 8);
      rcvBuf += 8;
      memcpy(CheckVal, rcvBuf, 8);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetRsaMainKey(char *index,char *RsaKey)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "03",
                    HDT_ASC, index, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      memcpy(RsaKey, rcvBuf + 4, 256);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetRsaPubKey(char *index,char *RsaKey)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "04",
                    HDT_ASC, index, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      memcpy(RsaKey, rcvBuf + 4, 256);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetWorkerKey(char *index,char *PinKey,char *PinCheckValue,char *Mackey,char *MacCheckValue)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "13",
                    HDT_ASC, index, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(PinKey, rcvBuf, 32);
      rcvBuf += 32;
      memcpy(PinCheckValue, rcvBuf, 8);
      rcvBuf += 8;
      memcpy(Mackey, rcvBuf, 32);
      rcvBuf += 32;
      memcpy(MacCheckValue, rcvBuf, 8);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetUserPIN(char *indexA,char *userId,char *pinData, char *indexB, char *newPinData)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "A1",
                    HDT_ASC, indexA, 5,
                    HDT_LLA, userId, strlen(userId),
                    HDT_LLL, pinData, strlen(pinData),
                    HDT_ASC, indexB, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(newPinData, rcvBuf, 64);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}


int Hsm2GetPINRsa(char *indexA,char *pan,char *pin, char *indexB, char *fA, char *fB, char *fC, char *acctNo, char *newPin)
{
   int   res;
   char  *sndBuf, *rcvBuf;
   char  factor[49];

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   memset(factor, 0, sizeof(factor));
   memset(factor, 'F', 48);
   memcpy(factor + 00, fA, strlen(fA));
   memcpy(factor + 16, fB, strlen(fB));
   memcpy(factor + 32, fC, strlen(fC));

   res = HsmProcess(sndBuf, rcvBuf, "B1",
                    HDT_ASC, indexA, 5,
                    HDT_ASC, pan, 16,
                    HDT_LLL, pin, strlen(pin),
                    HDT_ASC, indexB, 5,
                    HDT_ASC, factor, 48,
                    HDT_LLA, acctNo, strlen(acctNo),
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(newPin, rcvBuf, 16);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetPINRsa2(char *indexA,char *pan,char *pin, char *indexB, char *workKey, char *pinType, char *newPin)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "B2",
                    HDT_ASC, indexA, 5,
                    HDT_LLA, pan, strlen(pan),
                    HDT_LLL, pin, strlen(pin),
                    HDT_ASC, indexB, 5,
                    HDT_ASC, workKey, 32,
                    HDT_ASC, pinType, 1,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(newPin, rcvBuf, 16);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}


int Hsm2GetMac(char *indexA, char *workKey, char *data, int len, char *mac)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   //ELOG(DEBUG, "MACDATA(%s)", data);

   res = HsmProcess(sndBuf, rcvBuf, "27",
                    HDT_ASC, indexA, 5,
                    HDT_ASC, workKey, 32,
                    HDT_LLB, data, len,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(mac, rcvBuf, 8);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetMacX99(char *indexA, char *workKey, char *data, int len, char *mac)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "23",
                    HDT_ASC, indexA, 5,
                    HDT_ASC, workKey, 32,
                    HDT_LLB, data, len,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      strToBin(rcvBuf,mac,8);

//      memcpy(mac, rcvBuf, 8);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetPsamKey(char *indexA, char *branchId, char *psamId, char *indexB, char *pkey, char *mac)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }


   res = HsmProcess(sndBuf, rcvBuf, "28",
                    HDT_ASC, indexA, 5,
                    HDT_ASC, branchId, 8,
                    HDT_ASC, psamId, 16,
                    HDT_ASC, indexB, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(pkey, rcvBuf, 32);
      rcvBuf += 32;
      memcpy(mac, rcvBuf, 8);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm2GetTrackData(char *masterKeyIdx, char *trkWorkKey, char *trackData, char *newTrackData)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "29",
                    HDT_ASC, masterKeyIdx, 5,
                    HDT_LLA, trkWorkKey, strlen(trkWorkKey),
                    HDT_LLB, trackData, strlen(trackData),
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if(strncmp(rcvBuf + 2, "00", 2) == 0)
   {
      rcvBuf += 4;
      memcpy(newTrackData, rcvBuf, strlen(rcvBuf));
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

//转加密用户密码2
int Hsm2CalcPinRsa(char *rsaIndex, char *rsaPin, char *pinIndex, char *panData, char *fA, char *fB, char *fC, char *newPinData)
{
   int       res;
   char      *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "B3",
                    HDT_ASC, rsaIndex, 5,
                    HDT_ASC, rsaPin, 256,
                    HDT_ASC, pinIndex, 5,
                    HDT_LLA, panData, 16,
                    HDT_ASC, fA, 8,
                    HDT_ASC, fB, 8,
                    HDT_ASC, fC, 8,
                    HDT_NULL);

   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if(strncmp(rcvBuf + 2, "00", 2) == 0)
   {
      rcvBuf += 4;
      memcpy(newPinData, rcvBuf, strlen(rcvBuf));
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int  Hsm2GetPINRsa3(char *indexA,char *pinData, char *newPinData)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "连接系统加密机(%s)失败, 错误代码:%d", HsmServerInfo, res);
         return res;
      }
   }

   res = HsmProcess(sndBuf, rcvBuf, "B4",
                    HDT_ASC, indexA, 5,
                    HDT_LLL, pinData, strlen(pinData),
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "系统加密机处理失败,错误码:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      memcpy(newPinData, rcvBuf, 6);
      return 0;
   }

   ELOG(ERROR, "系统加密机处理失败,系统加密机返回的错误码为(%s)", rcvBuf);

   return TTS_EGENERAL;
}
