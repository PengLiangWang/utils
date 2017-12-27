#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <curses.h>
#include <unistd.h>


#include "transrecords.h"

#define WIN_MAX_LINE    27  //屏幕的最大行数
#define WIN_LEN         20  //空白区域的最大行数
#define DISPLAY_LINE    20  //显示区域的最大行数
#define DISPLAY_COLS    100 //最大列数

static WINDOW        *win , *datewin ;
static char           caMonFile[80];
static int            mode = 0 ;
static TransRecords  *TransRecord;


char *c_trans_type[][2]=
{
    {"LOGIN       ",""},
    {"LOGOUT      ",""},
    {"SETTLE      ",""},
    {"BATCH       ",""},
    {"DOWN        ",""},
    {"ECHO        ",""},
    {"UPLOAD      ",""},
    {"BALANCE     ",""},
    {"PCA         ",""},
    {"PCC         ",""},
    {"PPT         ",""},
    {"PPC         ",""},
    {"RFD         ",""},
    {"ATZ         ",""},
    {"ATR         ",""},
    {"ATC         ",""},
    {"ACR         ",""},
    {"ACP         ",""},
    {"ACC         ",""},
    {"APT         ",""},
    {"APC         ",""}
};
static int  format_info(mon_data_type * mon,char * disp_buf);
static int  disp_date();
static int  head_disp();

static int disp_date()
{
    long   clock;
    struct tm *cur_date;
    char   buf[70] ;

    clock = time((long *)0);
    cur_date = localtime(&clock);
    memset(buf, 0, sizeof(buf));
    sprintf(buf,"%04d/%02d/%02d %02d:%02d:%02d ",
            cur_date->tm_year+1900,cur_date->tm_mon+1,cur_date->tm_mday,
            cur_date->tm_hour,cur_date->tm_min,cur_date->tm_sec) ;

    buf[strlen(buf)] = 0 ;
    mvaddstr(1, 11, buf);  //显示时间的坐标
    refresh() ;

    return 0 ;
}

static int head_disp()
{
    FILE    *fd;
    int      i ;
    char     caBuf[102];

    memset(caMonFile,0,sizeof(caMonFile));
    sprintf(caMonFile, "%s/etc/moni.en", getenv("HOME"));

    if( (fd = fopen(caMonFile, "r")) == NULL )
    {
        printf("can't find file %s", caMonFile);
        exit(0);
    }
    clear();
    i = 0;
    memset(caBuf, 0, sizeof(caBuf));
    while( !feof(fd) )
    {
        if (i >= WIN_MAX_LINE)
        {
            break;
        }
        fgets( caBuf, sizeof(caBuf), fd);
        mvaddstr(i,0, caBuf);
        i++;
    }
    fclose(fd);
    refresh();
    return 0;
}


static int format_info(mon_data_type * mon,  char * disp_buf)
{
    int       i;
    char      tempBuf[40];
    double    dValue;

    if (mon->flag == 0)
    {
        memset(disp_buf, ' ', DISPLAY_COLS) ;
        disp_buf[DISPLAY_COLS] = 0 ;
        return 0;
    }

    // 格式化数据
    i = 0;
    // 交易类型 8 BYTE
    memcpy(disp_buf + i, c_trans_type[mon->transType - 1][0], 8);
    i += 8 + 1;

    // 时间 8 BYTE
    memcpy(disp_buf + i, mon->trans_time, 2);
    i += 2;
    memcpy(disp_buf + i, ":",1);
    i += 1;
    memcpy(disp_buf + i, mon->trans_time + 2 ,2);
    i += 2;
    memcpy(disp_buf + i, ":", 1);
    i += 1;
    memcpy(disp_buf + i, mon->trans_time + 4 ,2);
    i += 2 + 1;

    // 商户号 16 BYTE
    memcpy(disp_buf + i, mon->mid, 15);
    i += 15 + 1;

    // 卡号 14 BYTE
    memcpy(disp_buf + i, mon->card_no, 6);
    i += 6;
    memcpy(disp_buf + i, "****", 4);
    i += 4;
    memcpy(disp_buf + i, mon->card_no + strlen(mon->card_no) - 4, 4);
    i += 4 + 1;


    // 交易金额 9 BYTE
    if ( strncmp(mon->amount, "000000000000", 12) )
    {
        dValue = strtod(mon->amount, NULL);
        snprintf(tempBuf, 10, "% 9.2f", dValue / 100.00);
        memcpy(disp_buf + i, tempBuf, 9);
        i += 9 + 1;
    }
    else
    {
        memcpy(disp_buf + i, "     0.00", 9);
        i += 9 + 1;
    }

    // 交易时长 5 BYTE
    /*
    if ( mon->flag == 2 )
    {
        snprintf(tempBuf, 5, "%.2f", (double)mon->total_time / 1000.00);
        memcpy(disp_buf + i, tempBuf, 5);
        i += 5 + 1;
    }
    else
    {
        i += 5 + 1;
    }
    */
    //i += 5 + 1;

    // 交易返回码 2 BYTE
    memcpy(disp_buf + i, mon->response, 2);
    i += 2 + 1;

    // 交易信息26个字节
    if ( strncmp(mon->response , "-1", 2) == 0 )
    {
        memcpy(disp_buf + i, "doing...", 8);
    }
    else if ( strncmp(mon->response , "00", 2) == 0 )
    {
        memcpy(disp_buf + i, "SUCCESS", 7);
    }
    else
    {
        memcpy(disp_buf + i, "ERROR", 5);
    }

    // 处理字符  0x00
    for(i = 0; i < DISPLAY_COLS; i++)
    {
        if(disp_buf[i] == 0)
        {
            disp_buf[i] = 0x20;
        }
    }
    disp_buf[DISPLAY_COLS] = 0;
}

int main(int argc, char *argv[])
{
    int    i ;
    char   last_line[DISPLAY_COLS + 1];
    int    curLine;
    char  *term = NULL;

    if ( argc == 2 && strncmp(argv[1], "init", 4) == 0)
    {
        void   *ptr;
        extern int errno;
        char   caFileName[128];

        memset(caFileName, 0x00, sizeof(caFileName));
        sprintf(caFileName, "%s/etc/TransRecords",getenv("HOME"));
        if ( access(caFileName, 0) == -1 )
        {
            char command[128];
            memset(caFileName, 0x00, sizeof(caFileName));
            sprintf(caFileName, "%s/etc",getenv("HOME"));
            mkdir(caFileName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            memset(caFileName, 0x00, sizeof(caFileName));
            sprintf(caFileName, "%s/etc/TransRecords",getenv("HOME"));
            sprintf(command, "echo `date` > %s", caFileName);
            system(command);
            sleep(1);
        }

        i = TransRecordsInit(1, &ptr);
        if ( i )
        {
            fprintf(stderr,"创建监控共享内存失败(%d) (ERRNO:%d) (%s)\n", i, errno, strerror(errno));
            exit(0);
        }
        exit(0);
    }
    else if ( argc == 2 && strncmp(argv[1], "close", 5) == 0)
    {
        if ( !TransRecordsOpen((void **)&TransRecord) )
        {
            TransRecordsDeatory();
        }
        exit(0);
    }

    if ( TransRecordsOpen((void **)&TransRecord) )
    {
        fprintf(stderr,"打开监控共享内存失败....\n");
        exit(0);
    }

    /*
    term = (char *)ttyname(0);
    if( !strncmp(term,"/dev/ttyp",9) )
    {
        putenv("TERM=vt100");
    }
    else
    {
        putenv("TERM=ansi");
    }
    */

    signal(SIGHUP,  SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGPIPE, SIG_IGN );

    // 初始化屏幕
    initscr();

    cbreak();

    noecho();

    head_disp();

    refresh() ;

    disp_date() ;

    win = newwin(WIN_LEN, DISPLAY_COLS, 5, 0);
    TransRecord->change = 1 ;
    curLine = 0;

    for ( ; ; )
    {
        nodelay(win, TRUE);
        sleep(1);
        i = wgetch(win);
        if(i == (int)'q' || i == (int)'Q' || i == 6 || i == 27 )           /* Ctrl+F */
        {
            endwin();
            nocbreak();
            echo();
            reset_shell_mode();
            TransRecordsClose((void **)&TransRecord);
            return 0;
        }

        disp_date() ;
        //  change 记录当前已经显示的行数
        if(TransRecord->change != curLine)
        {
          int   beginline ;
          curLine = TransRecord->change ;
          beginline = MAX_TRANS_NUMBER - DISPLAY_LINE ;
          for(i = 0; i < DISPLAY_LINE ; i++)
          {
            memset(last_line, 0x20, sizeof(last_line));
            format_info( &TransRecord->mon[i + beginline], last_line );
            wmove(win, i, 0);
            wprintw(win, last_line);
          }
          wrefresh(win);
        }
    }
}




