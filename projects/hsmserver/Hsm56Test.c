#include "ttsys.h"
#include "HsmAPI2.h"
#include <openssl/rsa.h>


int main(int argc,char *argv[])
{
   int    res;
   char   hsmHost[32], masterKey[33], checkVal[5];
   char   keyIndex[6];
   int    action ;

   memset(hsmHost,   0, sizeof(hsmHost));
   memset(keyIndex,  0, sizeof(keyIndex));
   memset(masterKey, 0, sizeof(masterKey));
   memset(checkVal,  0, sizeof(checkVal));

   if ( argc < 2 )
   {
      printf("Hsm56 get|set host index\n");
      exit(0);
   }

   if ( strcmp(argv[1], "set") == 0 && argc == 6)
   {
      action = 1;
      strncpy(hsmHost, argv[2], 30);
      strncpy(keyIndex, argv[3], 5);
      strncpy(masterKey, argv[4], 32);
      strncpy(checkVal, argv[5], 4);
      if ( strlen(masterKey) != 32 && strlen(checkVal) != 4 )
      {
         printf("key len = 32 , check value len = 4\n");
         exit(0);
      }
   }
   else if ( strcmp(argv[1], "get") == 0 && argc == 4 )
   {
      action = 0;
      strncpy(hsmHost, argv[2], 30);
      strncpy(keyIndex, argv[3], 5);
   }
   else 
   {
      printf("Hsm56 set host index key checkval\n");
      printf("Hsm56 get host index\n");
      exit(0);
   }


   res = Hsm56SetServer(hsmHost);
   if( res )
   {
      printf("hsm host error (%s)\n", hsmHost);
      return res;
   }

   if ( !action )
   {
      res = Hsm56GetMasterKey(keyIndex, masterKey, checkVal);
      if ( res )
      {
         printf(" Get MasterKey error \n");
         exit(0);
      }
   }
   else
   {
      res = Hsm56SetMasterKey(keyIndex, masterKey, checkVal);
      if ( res )
      {
         printf(" Set MasterKey error \n");
         exit(0);
      }
   }

   printf("KEY (%s)(%s)\n", masterKey, checkVal);

   return 0;
}
