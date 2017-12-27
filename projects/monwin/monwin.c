#include "monpub.h"

int main(int argc, char **argv)
{
    int i,iRet;

    for (i=3;i<35;i++) signal(i,SIG_IGN);
    signal(SIGHUP,cps_monend);
    signal(SIGINT,cps_monend);

    eloginit(LOG_LEVEL_DEBUG, "MonWin", NULL, "Monwin", 1, NULL);    
    ELOG(INFO, "%s started....", argv[0]);

    iRet = cps_scrinit();
    if (iRet)
    {
        ELOG(ERROR, "SCR INIT IS [%d] \n", iRet);
        cps_monend(iRet);
    }

    iRet = cps_welmon();
    if (iRet)
    {
        ELOG(ERROR, "SCR WELCOM SHOW FAILED [%d] \n", iRet);
        cps_monend(iRet);
    }

    iRet = cps_refresh();
    if (iRet)
    {
        ELOG(ERROR, "SCR REFRESH SHOW FAILED [%d] \n", iRet);
        cps_monend(iRet);
    }

    cps_monend(iRet);

    return 0;
}
