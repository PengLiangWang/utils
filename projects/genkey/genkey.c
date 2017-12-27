#include "ttsys.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DesUtil.h"

static int mbpos_gen_rand_key(char *mKey, int flag, char *merchId, char *termId, char *wKey, char *wVal)
{
    static int iRandIdx = 0;  //随机因子
    long  t;  //时间
    long  iRand1, iRand2, iRand3;  //3个随机数
    char  caRandNum[65];  // 随机BUFFER
    char  caKey[33];   //

    time(&t);  //获取时间


    //获取随机数
    srand( t + iRandIdx++ );
    iRand1 = rand();

    srand( t + iRandIdx++ );
    iRand2 = rand();

    srand( t + iRandIdx++ );
    iRand3 = rand();

    //设置随机因子
    if ( iRandIdx > 9999999 )
    {
        iRandIdx = 0;
    }

    memset(caRandNum, 0, sizeof(caRandNum));
    sprintf(caRandNum, "%08ld%08ld%08ld%08ld%s%s%08ld", t, iRand1, iRand2, iRand3, merchId, termId, (long)getpid());
    // 计算MD5
    memset(caKey, 0, sizeof(caKey));
    MDString1(caRandNum, caKey);

    memset(caRandNum, 0, sizeof(caRandNum));
    strToBin(caKey, caRandNum, 16);

    // 计算 CheckValue
    memset(caKey, 0, sizeof(caKey) );
    if ( flag )
    {
        endesmBlock(caRandNum, DES_MODE_2, caKey , 8 );
    }
    else
    {
        endesmBlock(caRandNum, DES_MODE_1, caKey , 8 );
    }
    binToStr(caKey, wVal, 4);

    // 加密获取工作密钥
    endesmBlock(mKey, DES_MODE_2, caRandNum,16);
    if ( flag ) //双倍长密钥
    {
        binToStr(caRandNum, wKey, 16);
    }
    else  //单倍长密钥
    {
        binToStr(caRandNum, wKey, 8);
        binToStr(caRandNum, wKey + 16, 8);
    }

    return 0;

}


int main(int argc, char *argv[])
{
    char   merchId[16];
    char   termId[9];
    int    i;
    char   mkey[17];
    char   newKey[33];
    char   chkVal[9];

    memset(mkey, 0, sizeof(mkey));
    memcpy(mkey, "1234567890ABCDEF",16);

    for ( i = 1; i < 501; i++ )
    {
        memset(merchId, 0, sizeof(merchId));
        memset(termId, 0, sizeof(termId));
        sprintf(merchId, "18000007257%04d", i);
        sprintf(termId, "7257%04d", i);

        memset(newKey, 0, sizeof(newKey));
        memset(chkVal, 0, sizeof(chkVal));

        mbpos_gen_rand_key(mkey, 1, merchId, termId, newKey, chkVal);

        fprintf(stdout, "%15s %8s %32s %8s\n", merchId, termId, newKey, chkVal);
        continue;
    }

    return 0;

}

