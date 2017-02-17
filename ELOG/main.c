#include  <stdio.h>
#include  <stdlib.h>
#include  "elog.h"

int main(int argc, char *argv[])
{
    printf("进入elog验证函数\n");

    ELOG(INFO, "test");

    return 0;
}

