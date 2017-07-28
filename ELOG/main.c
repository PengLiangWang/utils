#include  <stdio.h>
#include  <stdlib.h>
#include  "elog.h"

int main(int argc, char *argv[])
{
    eloginit( LOG_LEVEL_ERROR , "testlogyyy", "testlog/", "testlogyyy", LOG_FILE_MASK , NULL);
    //eloginit( LOG_LEVEL_DEBUG , "testlogyyy", NULL, "testlogyyy", LOG_HOST_MASK , "192.168.1.21:22");
    printf("进入elog验证函数\n");

    ELOG(DEBUG, "debug20170221testwpl");
    ELOG(WARN, "warn20170221testwpl");
    ELOG(INFO, "info20170221testwpl");
    ELOG(ERROR, "error20170221testwpl");

    return 0;
}
