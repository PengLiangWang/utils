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
  {"签到        ",""},
  {"签退        ",""},
  {"结帐        ",""},
  {"批上送      ",""},
  {"下装参数    ",""},
  {"回响测试    ",""},
  {"上送状态    ",""},
  {"查询余额    ",""},
  {"消费        ",""},
  {"消费冲正    ",""},
  {"消费取消    ",""},
  {"撤消冲正    ",""},
  {"退货        ",""},
  {"预授权      ",""},
  {"授权冲正    ",""},
  {"授权取消    ",""},
  {"授取冲正    ",""},
  {"授权完成    ",""},
  {"授完冲正    ",""},
  {"授完取消    ",""},
  {"授完取冲    ",""}
};

char *c_msg_text[][2] =
{
    {"00", "交易成功"},
    {"01", "查询发卡方"},
    {"02", "查询发卡方的特殊规定"},
    {"03", "无效商户"},
    {"04", "此卡被没收"},
    {"05", "持卡人身份认证失败"},
    {"06", "未知错误06"},
    {"07", "按特殊规定没收卡"},
    {"08", "未知错误08"},
    {"09", "未知错误09"},
    {"10", "部分承兑"},
    {"11", "成功，VIP 客户"},
    {"12", "无效交易"},
    {"13", "无效金额"},
    {"14", "无效卡号"},
    {"15", "此卡无对应发卡方"},
    {"16", "未知错误16"},
    {"17", "未知错误17"},
    {"18", "未知错误18"},
    {"19", "请重新办理交易"},
    {"20", "无效响应"},
    {"21", "该卡未初始化或睡眠卡"},
    {"22", "操作有误，或超出交易允许天数"},
    {"23", "不能接受的交易费"},
    {"24", "未知错误24"},
    {"25", "不能在文件上找到记录"},
    {"26", "未知错误26"},
    {"27", "未知错误27"},
    {"28", "未知错误28"},
    {"29", "未知错误29"},
    {"30", "格式错误"},
    {"31", "未知错误31"},
    {"32", "未知错误32"},
    {"33", "过期卡请予以没收"},
    {"34", "舞弊嫌疑请没收卡"},
    {"35", "请代理方联系并没收卡"},
    {"36", "卡被止付请没收卡"},
    {"37", "受卡方呼安全保密部门(没收卡)"},
    {"38", "PIN输入次数超限没收卡"},
    {"39", "无此信用卡帐户"},
    {"40", "请求的功能不支持"},
    {"41", "遗失卡请予以没收"},
    {"42", "无此帐户"},
    {"43", "此卡被偷请予以没收"},
    {"44", "无投资帐户"},
    {"45", "未知错误45"},
    {"46", "未知错误46"},
    {"47", "未知错误47"},
    {"48", "未知错误48"},
    {"49", "未知错误49"},
    {"50", "未知错误50"},
    {"51", "资金不足"},
    {"52", "无此支票帐户"},
    {"53", "无储蓄帐户"},
    {"54", "过期卡请勿交易"},
    {"55", "密码错误"},
    {"56", "无卡记录"},
    {"57", "不允许持卡人进行的交易"},
    {"58", "不允许终端进行交易"},
    {"59", "卡片校验错，舞弊嫌疑请勿交易"},
    {"60", "请受卡方与安全保密部门联系"},
    {"61", "超出允许的交易限额"},
    {"62", "受限制的卡"},
    {"63", "违反安全保密规定"},
    {"64", "原始金额错误"},
    {"65", "超出允许的交易次数"},
    {"66", "请通知安全部门"},
    {"67", "强行扣留(没收该卡)"},
    {"68", "交易超时，请重试"},
    {"69", "未知错误69"},
    {"70", "未知错误70"},
    {"71", "未知错误71"},
    {"72", "未知错误72"},
    {"73", "未知错误73"},
    {"74", "未知错误74"},
    {"75", "PIN错误次数超限"},
    {"76", "未知错误76"},
    {"77", "未知错误77"},
    {"78", "未知错误78"},
    {"79", "未知错误79"},
    {"80", "未知错误"},
    {"81", "未知错误"},
    {"82", "未知错误"},
    {"83", "未知错误"},
    {"84", "未知错误"},
    {"85", "未知错误"},
    {"86", "未知错误"},
    {"87", "未知错误"},
    {"88", "未知错误"},
    {"89", "未知错误"},
    {"90", "主机正在日终处理"},
    {"91", "发卡方或交换站不起作用"},
    {"92", "通讯失败"},
    {"93", "交易不能完成违法"},
    {"94", "重复交易"},
    {"95", "调节控制错"},
    {"96", "系统故障"},
    {"97", "ATM或POS终端找不到"},
    {"98", "交换中心收不到发卡方应答"},
    {"99", "PIN格式错"}
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

    if ( mode == 0 )
    {
        memset(caMonFile,0,sizeof(caMonFile));
        sprintf(caMonFile, "%s/etc/POSM_CN_MODE_1", getenv("HOME"));
    }
    else if ( mode == 1 )
    {
        memset(caMonFile,0,sizeof(caMonFile));
        sprintf(caMonFile, "%s/etc/POSM_CN_MODE_1", getenv("HOME"));
    }

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

    if ( mon->flag == 0 )
    {
        memset(disp_buf, ' ', DISPLAY_COLS) ;
        disp_buf[DISPLAY_COLS] = 0 ;
        return 0;
    }


    if ( mode == 0 )
    {
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

        //交易结果
        // 交易返回码 2 BYTE
        memcpy(disp_buf + i, mon->response, 2);
        i += 2 + 1;


        // 交易信息26个字节
        if ( strncmp(mon->response , "-1", 2) == 0 )
        {
            memcpy(disp_buf + i, "处理中", 6);
        }
        else
        {
            if ( strncmp(mon->response , "99", 2) > 0 )
            {
                if ( strncmp(mon->response , "A0", 2) == 0 )
                {
                    strcpy(disp_buf + i, "MAC 校验错");
                }
                else if ( strncmp(mon->response , "A1", 2) == 0 )
                {
                    strcpy(disp_buf + i, "转账货币不一致");
                }
                else if ( strncmp(mon->response , "A2", 2) == 0 )
                {
                    strcpy(disp_buf + i, "交易成功，请向发卡行确认");
                }
                else if ( strncmp(mon->response , "A3", 2) == 0 )
                {
                    strcpy(disp_buf + i, "资金到账行无此账户");
                }
                else if ( strncmp(mon->response , "A4", 2) == 0 )
                {
                    strcpy(disp_buf + i, "交易成功，请向发卡行确认");
                }
                else if ( strncmp(mon->response , "A5", 2) == 0 )
                {
                    strcpy(disp_buf + i, "交易成功，请向发卡行确认");
                }
                else if ( strncmp(mon->response , "A6", 2) == 0 )
                {
                    strcpy(disp_buf + i, "交易成功，请向发卡行确认");
                }
                else if ( strncmp(mon->response , "A7", 2) == 0 )
                {
                    strcpy(disp_buf + i, "拒绝，交换中心异常");
                }
                else
                {
                    strcpy(disp_buf + i, "未知错误");
                }
            }
            else
            {
                strcpy(disp_buf + i, c_msg_text[atoi(mon->response)][1]);
            }
        }
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
        else if(i == ' ')
        {
            if ( mode == 0 )
            {
                mode = 1;
            }
            else if ( mode == 1)
            {
                mode = 0;
            }

            head_disp();

        }

        disp_date() ;

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




