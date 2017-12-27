
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
      ELOG(INFO, "(Event OnExit)�ر���56�����ܻ�(%s)������", HsmServerInfo);
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
      ELOG(ERROR, "��������serverInfo����ȷ.");
      return TTS_EINVAL;
   }

   memset(serverIP, 0, sizeof(serverIP));
   serverPort = 0;

   res = sscanf(HsmServerInfo, "%[^:]:%d", serverIP, &serverPort);
   if ( res != 2 )
   {
      ELOG(ERROR, "56�����ܻ�������Ϣ����ȷ(%s)", HsmServerInfo);
      return TTS_EINVAL;
   }

   res = SocketConnect(&HsmSocket, serverIP, serverPort);
   if ( res )
   {
      ELOG(ERROR, "����56�����ܻ�(%s)ʧ��,������Ϣ:%d,%s", HsmServerInfo, res, GetStrError(res));
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
         ELOG(ERROR, "����ͨѶ����TIMEOUT ʧ��,������Ϣ:%d,%s", res, GetStrError(res));
         SocketClose(&HsmSocket);
         return res;
      }

      HsmConnected = 1;
      ELOG(INFO, "��56�����ܻ�(%s)�ɹ��Խ�", HsmServerInfo);

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

   ELOG(INFO , "SEND TO 56 HSM (%d)(%s)", sndLen, HsmSendBuf + 2);

   res = send(HsmSocketHandle, HsmSendBuf, sndLen, 0);
   if ( res < 0 )
   {
      res = errno;
      ELOG(ERROR, "��56�����ܻ�ͨѶ(send)ʧ��,������Ϣ:%d,%s", res, GetStrError(res));
      SocketClose(&HsmSocket);
      HsmConnected = 0;

      /** �������ʧ��,��������һ�β�����*/

      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "��������56�����ܻ�(%s)ʧ��, �������:%d", HsmServerInfo, res);
         return res;
      }

      res = send(HsmSocketHandle, HsmSendBuf, sndLen, 0);
      if ( res < 0 )
      {
         res = errno;
         ELOG(ERROR, "��56�����ܻ�ͨѶ(send)ʧ��,������Ϣ:%d,%s", res, GetStrError(res));
         SocketClose(&HsmSocket);
         HsmConnected = 0;
         return res;
      }
   }

   res = recv(HsmSocketHandle, HsmRecvBuf, 2, 0);
   if ( res < 0 )
   {
      res = errno;
      ELOG(ERROR, "��56�����ܻ�ͨѶ(recv)ʧ��,������Ϣ:%d,%s", res, GetStrError(res));
      SocketClose(&HsmSocket);
      HsmConnected = 0;
      return res;
   }

   *rcvLen = (int)((HsmRecvBuf[0] & 0xFF) * 0xFF ) + (int)(HsmRecvBuf[1] & 0xFF);
   if ( *rcvLen <= 0 )
   {
      ELOG(ERROR, "��56�����ܻ�ͨѶ(recv)ʧ��,���ݳ��ȷǷ�");
      SocketClose(&HsmSocket);
      HsmConnected = 0;
      return TTS_EINVAL;
   }

   res = recv(HsmSocketHandle, HsmRecvBuf + 2, *rcvLen, 0);
   if ( res < 0 )
   {
      res = errno;
      ELOG(ERROR, "��56�����ܻ�ͨѶ(recv)ʧ��,������Ϣ:%d,%s", res, GetStrError(res));
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
         ELOG(WARN, "�������ݽ���....");
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
            ELOG(ERROR, "�������Ͳ����϶���(%c)", valType);
            va_end(args);
            return TTS_EINVAL;
      }
   }

   va_end(args);

   return HsmCall(sndLen, &rcvLen);
}

int Hsm56SetServer(char *serverInfo)
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
      ELOG(ERROR, "56�����ܻ�������Ϣ����ȷ(%s)", serverInfo);
      return TTS_EINVAL;
   }
   memset(HsmServerInfo, 0, sizeof(HsmServerInfo));
   strcpy(HsmServerInfo, serverInfo);

   return 0;
}

int Hsm56Close()
{
   if ( HsmSocket )
   {
      ELOG(INFO, "�ر���56�����ܻ�(%s)������", HsmServerInfo);
      SocketClose(&HsmSocket);
   }

   HsmConnected = 0;

   return 0;
}



int Hsm56GetMac(char *indexA, char *workKey, char *data, int len, char *mac)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "����56�����ܻ�(%s)ʧ��, �������:%d", HsmServerInfo, res);
         return res;
      }
   }

   ELOG(INFO , "56 MACDATA(%s)", data);

   res = HsmProcess(sndBuf, rcvBuf, "80",
                    HDT_ASC, indexA, 3,
                    HDT_ASC, workKey, 16,
                    HDT_LLB, data, len,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "56�����ܻ�����ʧ��,������:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      strToBin(rcvBuf, mac, 8);
      return 0;
   }

   ELOG(ERROR, "56�����ܻ�����ʧ��,56�����ܻ����صĴ�����Ϊ(%s)", rcvBuf);

   return TTS_EGENERAL;
}


int Hsm56GetMasterKey(char *indexA, char *data, char *mac)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "����56�����ܻ�(%s)ʧ��, �������:%d", HsmServerInfo, res);
         return res;
      }
   }

   ELOG(DEBUG, "56 MACDATA(%s)", data);

   res = HsmProcess(sndBuf, rcvBuf, "E6",
                    HDT_ASC, indexA, 5,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "56�����ܻ�����ʧ��,������:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      rcvBuf += 4;
      strncpy(data, rcvBuf, 32);
      rcvBuf += 32;
      strncpy(mac, rcvBuf, 4);
      return 0;
   }

   ELOG(ERROR, "56�����ܻ�����ʧ��,56�����ܻ����صĴ�����Ϊ(%s)", rcvBuf);

   return TTS_EGENERAL;
}

int Hsm56SetMasterKey(char *indexA, char *data, char *mac)
{
   int   res;
   char  *sndBuf, *rcvBuf;

   HsmResetBuffer(&sndBuf, &rcvBuf);

   if ( !HsmConnected )
   {
      res = HsmConnect();
      if ( res )
      {
         ELOG(ERROR, "����56�����ܻ�(%s)ʧ��, �������:%d", HsmServerInfo, res);
         return res;
      }
   }

   ELOG(DEBUG, "56 MACDATA(%s)", data);

   res = HsmProcess(sndBuf, rcvBuf, "E2",
                    HDT_ASC, indexA, 5,
                    HDT_ASC, data, 32,
                    HDT_ASC, mac, 4,
                    HDT_NULL);
   if ( res )
   {
      ELOG(ERROR, "56�����ܻ�����ʧ��,������:%d", res);
      return res;
   }

   if ( strncmp(rcvBuf + 2, "00", 2) == 0 )
   {
      return 0;
   }

   ELOG(ERROR, "56�����ܻ�����ʧ��,56�����ܻ����صĴ�����Ϊ(%s)", rcvBuf);

   return TTS_EGENERAL;
}

