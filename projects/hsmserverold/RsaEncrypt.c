#include "ttsys.h"
#include "HsmAPI.h"
#include <openssl/rsa.h>

int main(int argc,char *argv[])
{
   RSA   *rsa;
   uchar  rsaN[129];
   uchar  inData[129];
   uchar  outData[129];
   uchar  strData[257];
   int    n,dataLen ;

   if (argc != 3 || strlen(argv[1]) != 256 || strlen(argv[2]) > 100)
   {
      printf("Usage: RsaEn PublicKey(=256) Data(<100)\n");
      return -1;
   }

   memset(rsaN, 0, sizeof(rsaN) );
   memset(inData, 0, sizeof(inData) );
   memset(outData, 0, sizeof(outData) );
   memset(strData, 0, sizeof(strData) );

   strToBin(argv[1], rsaN, 128);
   dataLen = strlen(argv[2]);
   strcpy(inData, argv[2]);

   rsa = RSA_new();
   if( NULL == rsa )
   {
      printf( "main.RSA_new for encrypt\n" );
      return -1;
   }

   // ÉèÖÃ¹«Ô¿
   rsa->n = BN_bin2bn( rsaN, 128, rsa->n );
   rsa->e = BN_bin2bn( "\x01\x00\x01", 3, rsa->e );

   n = RSA_public_encrypt( dataLen, inData, outData, rsa, RSA_PKCS1_PADDING );
   if ( n == -1 )
   {
      printf( "main.RSA_public_encrypt error\n" );
      RSA_free( rsa );
      return -1;
   }

   binToStr(outData, strData, RSA_size( rsa ));
   
   RSA_free( rsa );
   
   printf("data(%d) = [%s]\n", n, strData);

   return 0;
}



