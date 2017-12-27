#ifndef _MONWIN_INCLUDE_H_
#define _MONWIN_INCLUDE_H_

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <memory.h>
#include <malloc.h>
#include <signal.h>
#include <termio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <curses.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/select.h>

#include "md5.h"
#include "elog.h"

#define MON_CON_FILE "sms.ini"
#define MON_CON_HEAD "[Monitor Info]"

#define TIME_OUT 60

typedef struct Disp_tm
{
    char caDay[10+1];
    char caDate[4+1];
    char caTime[8+1];
    char caDmac[16+1];
} Disptime;

/*
typedef struct Disp_trace
{
    char caIndex[11+1];
    long lNodeaddr;
    long lNodebit;
    char caStat[4+1];
    long lInfoaddr;
    long lInfobit;
    char caUsercode[11+1];
    char caUsername[20+1];
} Disptrace;
*/

typedef struct MSGQMON
{
    long msgtype;
    char info[80];
} MsgQueMon;

static int iMsg;
static int iTrans;
static long lRows;
//static WINDOW *win_mon;

int cps_scrinit();
int cps_welmon();
int cps_refresh();
void cps_monend(int iSig);
#endif
