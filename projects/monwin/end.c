#include "monpub.h"
#include "ttsys.h"
#include "act/DB_PosJnls.h"
#include "datetime.h"


void cps_scrend()
{
    msgctl(iMsg,IPC_RMID,0);
    refresh();
    endwin();
}

void cps_monend(int iSig)
{
    cps_scrend();
    switch(iSig)
    {
        case SIGHUP:
            ELOG(INFO, "TERM HANG UP!\n");
            break;
        case SIGINT:
            ELOG(INFO, "USER COMM EXIT!\n");
            break;
        default:
            ELOG(ERROR, "NOT KNOWN ERROR!\n");
    }

    exit(0);
}


