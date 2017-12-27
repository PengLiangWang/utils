
#include <stdio.h>
#include <string.h>
#include <openssl/rsa.h>

void print_buf_hex( unsigned char *buf, int len )
{
    int i;

    for( i = 0; i < len; i++ )
    {
        printf( "%02x", buf[i] );
    }
}

int main( int argc, char *argv[] )
{
    RSA           *rsa;
    int            n;
    unsigned char  text[128], cipher[128];
    unsigned char  rsa_n[128], rsa_d[128];

    // 生成密钥
    rsa = RSA_generate_key( 1024, RSA_F4, NULL, NULL );
    if( NULL == rsa )
    {
        printf( "main.RSA_generate_key\n" );
        return -1;
    }
    n = RSA_size( rsa );
    BN_bn2bin( rsa->n, rsa_n ); // 保存公钥
    BN_bn2bin( rsa->d, rsa_d ); // 保存私钥
    RSA_free( rsa );
    printf( "RSA_size: %d\n\npublic modulus:\n", n );
    print_buf_hex( rsa_n, sizeof(rsa_n) );
    printf( "\n\nprivate exponent:\n" );
    print_buf_hex( rsa_d, sizeof(rsa_d) );
    printf( "\n\n" );

    // 加密的过程
    rsa = RSA_new();
    if( NULL == rsa )
    {
        printf( "main.RSA_new for encrypt\n" );
        return -1;
    }
    // 设置公钥
    rsa->n = BN_bin2bn( rsa_n, sizeof(rsa_n), rsa->n );
    rsa->e = BN_bin2bn( "\x01\x00\x01", 3, rsa->e );
    // 设置明文并加密

    memset(text, 0, sizeof(text));
    memset(cipher, 0, sizeof(cipher));
    /*
    for( n = 0; n < sizeof(text); n++ )
    {
        text[n] = n;
    }
    */

    strncpy(text, "0987654321",10);
    //n = RSA_public_encrypt( sizeof(text), text, cipher, rsa, RSA_NO_PADDING );
    n = RSA_public_encrypt( strlen(text), text, cipher, rsa, RSA_PKCS1_PADDING );
    RSA_free( rsa );
    printf( "RSA_public_encrypt: %d\ncipher:\n", n );
    print_buf_hex( cipher, sizeof(cipher) );
    printf( "\n\n" );

    // 解密的过程
    rsa = RSA_new();
    if( NULL == rsa )
    {
        printf( "main.RSA_new for decrypt\n" );
        return -1;
    }
    // 设置公钥及私钥
    rsa->n = BN_bin2bn( rsa_n, sizeof(rsa_n), rsa->n );
    rsa->e = BN_bin2bn( "\x01\x00\x01", 3, rsa->e );
    rsa->d = BN_bin2bn( rsa_d, sizeof(rsa_d), rsa->d );
    // 解密数据
    n = RSA_private_decrypt( sizeof(cipher), cipher, text, rsa,
                             RSA_PKCS1_PADDING );
    RSA_free( rsa );
    printf( "RSA_private_decrypt: %d\ntext:\n", n );
    printf("[%s]\n",text);
    //print_buf_hex( text, sizeof(text) );
    putchar( '\n' );

    return 0;
}
