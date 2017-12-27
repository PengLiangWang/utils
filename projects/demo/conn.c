#include "ttsys.h"


int main(int argc, char **argv)
{
   char      sHost[16];
   char      sPort[7];
   char      sTime[7];
   int       iPort;
   int       iTime;
   TSocket  *sock;
   int       res;


   if ( argc < 3 )
   {
      printf("tconn host port timeout\n");
      exit(0);
   }

   memset(sHost, 0, sizeof(sHost));
   memset(sPort, 0, sizeof(sPort));
   memset(sTime, 0, sizeof(sTime));

   strncpy(sHost, argv[1], 15);
   strncpy(sPort, argv[2], 5);
   if ( argc == 4 )
   {
      strncpy(sTime, argv[3], 5);
      iTime = atoi(sTime);
   }
   else
   {
      iTime = 5;
   }

   iPort = atoi(sPort);

   res = SocketConnectEx(&sock, sHost, iPort, iTime);
   if ( res )
   {
      printf("connect %s:%d error(%d:%s)\n", sHost, iPort, res, GetStrError(res));
      return res;
   }
   printf("connect %s:%d OK.\n", sHost, iPort);
   getchar();
   SocketClose(&sock);
   return res;
}
