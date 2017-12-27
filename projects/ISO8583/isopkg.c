#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ISO8583.h"

int main(int argc , char *argv[])
{
    FILE    *dataFile;
    ISO8583  sISO8583;
    int      iRet,i,iLen;
    char     dataBuf[2048];
    char     ascBuf[1025];
    char     isoBuf[513];
    

    memset(&sISO8583, 0, sizeof(sISO8583));

    if ( argc != 2 )
    {
        printf("isopkg file\n");
        exit(0);
    }
    dataFile = fopen(argv[1], "r");
    if ( dataFile == NULL )
    {
        printf("ISO8583 数据文件 (%s) 不存在.\n", argv[1]);
        exit(0);
    }

    memset(dataBuf, 0, sizeof(dataBuf));
    memset(isoBuf, 0, sizeof(isoBuf));
    memset(ascBuf, 0, sizeof(ascBuf));

    iRet = fread(dataBuf, 1, 2048, dataFile);
    if ( iRet < 64 )
    {
        printf("ISO8583 数据文件 (%s) 不正确.\n", argv[1]);
        fclose(dataFile);
        exit(0);
    }
    fclose(dataFile);

    iLen = 0;

    for( i = 0 ; i < iRet + 1; i++)
    {
        if ( dataBuf[i] != ' ' && dataBuf[i] != '\n' && dataBuf[i] != '\r')
        {
            ascBuf[iLen++] = dataBuf[i];
        }
    }

    strToBin(ascBuf, isoBuf, iRet/2);
    
    iRet = ISO8583_ntoh(&sISO8583, isoBuf);
    if ( iRet != 0 )
    {
        printf("无法解析ISO8583数据\n");
        exit(0);
    }

    ISO8583_dump("ISOPKG", &sISO8583);
    
    exit(0);
}
