#include "ttsys.h"
#include "HsmAPI.h"
#include <openssl/rsa.h>

static int RsaGenerate(char *RsaPrivateKey, char *RsaPublicKey);

int main(int argc,char *argv[])
{
   RSA   *rsa;
   uchar  rsaN[129];
   uchar  inData[129];
   uchar  outData[129];
   uchar  strData[257];
   int    n,dataLen ;

   if ( argc > 1 && strcmp(argv[1], "get") == 0 )
   {
      char   privateKey[257],publicKey[257];

      memset(privateKey, 0, sizeof(privateKey));
      memset(publicKey, 0, sizeof(publicKey));

      n = RsaGenerate(privateKey, publicKey);
      if ( n )
      {
         return -1;
      }

      memset(inData, 0, sizeof(inData));
      memcpy(inData, privateKey, 64);
      printf("PRIVATE KEY(1) [%s]\n", inData);

      memset(inData, 0, sizeof(inData));
      memcpy(inData, privateKey + 64, 64);
      printf("PRIVATE KEY(2) [%s]\n", inData);

      memset(inData, 0, sizeof(inData));
      memcpy(inData, privateKey + 128, 64);
      printf("PRIVATE KEY(3) [%s]\n", inData);
      
      memset(inData, 0, sizeof(inData));
      memcpy(inData, privateKey + 192, 64);
      printf("PRIVATE KEY(4) [%s]\n", inData);

      memset(inData, 0, sizeof(inData));
      memcpy(inData, publicKey, 64);
      printf("PUBLIC KEY(1) [%s]\n", inData);

      memset(inData, 0, sizeof(inData));
      memcpy(inData, publicKey + 64, 64);
      printf("PUBLIC KEY(2) [%s]\n", inData);

      memset(inData, 0, sizeof(inData));
      memcpy(inData, publicKey + 128, 64);
      printf("PUBLIC KEY(3) [%s]\n", inData);
      
      memset(inData, 0, sizeof(inData));
      memcpy(inData, publicKey + 192, 64);
      printf("PUBLIC KEY(4) [%s]\n", inData);
      
      return 0;
   }

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

   // 设置公钥
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

static int RsaGenerate(char *RsaPrivateKey, char *RsaPublicKey)
{
  int           rv, iKeyLen;
  RSA          *rsa;
  uchar         rsaN[129],rsaD[129];
  

  memset(rsaN, 0, sizeof(rsaN));
  memset(rsaD, 0, sizeof(rsaD));

  rsa = RSA_generate_key( 1024, RSA_F4, NULL, NULL );
  if ( rsa == NULL )
  {
    printf("产生的密钥失败");
    return -1;
  }

  BN_bn2bin( rsa->n, rsaN ); // 保存公钥
  BN_bn2bin( rsa->d, rsaD ); // 保存私钥
  
  iKeyLen = RSA_size( rsa );

  RSA_free( rsa );

  binToStr(rsaD, RsaPrivateKey, iKeyLen);
  binToStr(rsaN, RsaPublicKey, iKeyLen);

  return 0;
}


