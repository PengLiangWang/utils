#include "ttsys.h"
#include "HsmAPI.h"
#include <openssl/rsa.h>


char  PublicKey[] = "98FD87C53F18F533E0265B00F0F87A10F450903960728B63504FB5A4012DCFB64BBD7599B63D341F247C070CFD73C9D72E5EA0F417C7247DBA34CE9DE5419215ADC6AA08A625F4F6D980A6427EBD6A54BFE1B0C5645DA4ED8CF544372092AE1540723BAEA7322573B372BB96B7C424549D05641EEC5AE0DE90144D29268C82F5";

char  PrivateKey[] = "64EAA80A866A92241BF1710A17E9EFAB0B5DB12507E7C6E645EDAE6576999F702259E11EF9C5398269709EBE04D9C2047D03D2ABF09B5E08C9315831732E638B80311F2C1E0CE25F74A3A0B3177BD316DDA99709A96C5506897C478B37F8D23B5D57079EF987C4DF439DFD0FEB04B4FECDCE3692E1FE51813FDAA23846F3B6E9";

char  EnData[] = "3452DE340237DB5847A75B84C7A6BD7876BFB11CE771E78EF103D5EF71BC6DA9B86B9F768C5DE17CF3CE919063D32A96D1B9102750151B7F340BD26614087645719CF8A3829FA8857A3FD2155F0FBB1276B00DA8F2CC7E13C7C09788D98011D91F2830052C365288B265C3EEBF949EEEACF9FC56DB56E554F005AC9710175663";

int main(int argc,char *argv[])
{
   RSA   *rsa;
   uchar  rsaN[129];
   uchar  inData[129];
   uchar  outData[129];
   uchar  strData[257];
   uchar  publicKey[129];
   uchar  privateKey[129];
   int    n,dataLen ;

   memset(inData, 0, sizeof(inData) );
   memset(outData, 0, sizeof(outData) );
   memset(strData, 0, sizeof(strData) );
   memset(publicKey, 0, sizeof(publicKey) );
   memset(privateKey, 0, sizeof(privateKey) );

   strToBin(PublicKey, publicKey, 128);
   strToBin(PrivateKey, privateKey, 128);
   strToBin(EnData, inData, 128);

   rsa = RSA_new();
   if( NULL == rsa )
   {
      printf( "main.RSA_new for encrypt\n" );
      return -1;
   }

   rsa->d = BN_bin2bn( privateKey, 128, rsa->d );
   rsa->n = BN_bin2bn( publicKey, 128, rsa->n );
   rsa->e = BN_bin2bn( "\x01\x00\x01", 3, rsa->e );

   n = RSA_private_decrypt( 128, inData, outData, rsa, RSA_PKCS1_PADDING );
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

