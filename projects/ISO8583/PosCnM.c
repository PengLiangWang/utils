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

#define WIN_MAX_LINE    27  //��Ļ���������
#define WIN_LEN         20  //�հ�������������
#define DISPLAY_LINE    20  //��ʾ������������
#define DISPLAY_COLS    100 //�������

static WINDOW        *win , *datewin ;
static char           caMonFile[80];
static int            mode = 0 ;
static TransRecords  *TransRecord;


char *c_trans_type[][2]=
{
  {"ǩ��        ",""},
  {"ǩ��        ",""},
  {"����        ",""},
  {"������      ",""},
  {"��װ����    ",""},
  {"�������    ",""},
  {"����״̬    ",""},
  {"��ѯ���    ",""},
  {"����        ",""},
  {"���ѳ���    ",""},
  {"����ȡ��    ",""},
  {"��������    ",""},
  {"�˻�        ",""},
  {"Ԥ��Ȩ      ",""},
  {"��Ȩ����    ",""},
  {"��Ȩȡ��    ",""},
  {"��ȡ����    ",""},
  {"��Ȩ���    ",""},
  {"�������    ",""},
  {"����ȡ��    ",""},
  {"����ȡ��    ",""}
};

char *c_msg_text[][2] =
{
    {"00", "���׳ɹ�"},
    {"01", "��ѯ������"},
    {"02", "��ѯ������������涨"},
    {"03", "��Ч�̻�"},
    {"04", "�˿���û��"},
    {"05", "�ֿ��������֤ʧ��"},
    {"06", "δ֪����06"},
    {"07", "������涨û�տ�"},
    {"08", "δ֪����08"},
    {"09", "δ֪����09"},
    {"10", "���ֳж�"},
    {"11", "�ɹ���VIP �ͻ�"},
    {"12", "��Ч����"},
    {"13", "��Ч���"},
    {"14", "��Ч����"},
    {"15", "�˿��޶�Ӧ������"},
    {"16", "δ֪����16"},
    {"17", "δ֪����17"},
    {"18", "δ֪����18"},
    {"19", "�����°�����"},
    {"20", "��Ч��Ӧ"},
    {"21", "�ÿ�δ��ʼ����˯�߿�"},
    {"22", "�������󣬻򳬳�������������"},
    {"23", "���ܽ��ܵĽ��׷�"},
    {"24", "δ֪����24"},
    {"25", "�������ļ����ҵ���¼"},
    {"26", "δ֪����26"},
    {"27", "δ֪����27"},
    {"28", "δ֪����28"},
    {"29", "δ֪����29"},
    {"30", "��ʽ����"},
    {"31", "δ֪����31"},
    {"32", "δ֪����32"},
    {"33", "���ڿ�������û��"},
    {"34", "���������û�տ�"},
    {"35", "�������ϵ��û�տ�"},
    {"36", "����ֹ����û�տ�"},
    {"37", "�ܿ�������ȫ���ܲ���(û�տ�)"},
    {"38", "PIN�����������û�տ�"},
    {"39", "�޴����ÿ��ʻ�"},
    {"40", "����Ĺ��ܲ�֧��"},
    {"41", "��ʧ��������û��"},
    {"42", "�޴��ʻ�"},
    {"43", "�˿���͵������û��"},
    {"44", "��Ͷ���ʻ�"},
    {"45", "δ֪����45"},
    {"46", "δ֪����46"},
    {"47", "δ֪����47"},
    {"48", "δ֪����48"},
    {"49", "δ֪����49"},
    {"50", "δ֪����50"},
    {"51", "�ʽ���"},
    {"52", "�޴�֧Ʊ�ʻ�"},
    {"53", "�޴����ʻ�"},
    {"54", "���ڿ�������"},
    {"55", "�������"},
    {"56", "�޿���¼"},
    {"57", "������ֿ��˽��еĽ���"},
    {"58", "�������ն˽��н���"},
    {"59", "��ƬУ����������������"},
    {"60", "���ܿ����밲ȫ���ܲ�����ϵ"},
    {"61", "��������Ľ����޶�"},
    {"62", "�����ƵĿ�"},
    {"63", "Υ����ȫ���ܹ涨"},
    {"64", "ԭʼ������"},
    {"65", "��������Ľ��״���"},
    {"66", "��֪ͨ��ȫ����"},
    {"67", "ǿ�п���(û�ոÿ�)"},
    {"68", "���׳�ʱ��������"},
    {"69", "δ֪����69"},
    {"70", "δ֪����70"},
    {"71", "δ֪����71"},
    {"72", "δ֪����72"},
    {"73", "δ֪����73"},
    {"74", "δ֪����74"},
    {"75", "PIN�����������"},
    {"76", "δ֪����76"},
    {"77", "δ֪����77"},
    {"78", "δ֪����78"},
    {"79", "δ֪����79"},
    {"80", "δ֪����"},
    {"81", "δ֪����"},
    {"82", "δ֪����"},
    {"83", "δ֪����"},
    {"84", "δ֪����"},
    {"85", "δ֪����"},
    {"86", "δ֪����"},
    {"87", "δ֪����"},
    {"88", "δ֪����"},
    {"89", "δ֪����"},
    {"90", "�����������մ���"},
    {"91", "�������򽻻�վ��������"},
    {"92", "ͨѶʧ��"},
    {"93", "���ײ������Υ��"},
    {"94", "�ظ�����"},
    {"95", "���ڿ��ƴ�"},
    {"96", "ϵͳ����"},
    {"97", "ATM��POS�ն��Ҳ���"},
    {"98", "���������ղ���������Ӧ��"},
    {"99", "PIN��ʽ��"}
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
    mvaddstr(1, 11, buf);  //��ʾʱ�������
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
        // ��ʽ������
        i = 0;
        // �������� 8 BYTE
        memcpy(disp_buf + i, c_trans_type[mon->transType - 1][0], 8);
        i += 8 + 1;

        // ʱ�� 8 BYTE
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

        // �̻��� 16 BYTE
        memcpy(disp_buf + i, mon->mid, 15);
        i += 15 + 1;

        // ���� 14 BYTE
        memcpy(disp_buf + i, mon->card_no, 6);
        i += 6;
        memcpy(disp_buf + i, "****", 4);
        i += 4;
        memcpy(disp_buf + i, mon->card_no + strlen(mon->card_no) - 4, 4);
        i += 4 + 1;


        // ���׽�� 9 BYTE
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

        //���׽��
        // ���׷����� 2 BYTE
        memcpy(disp_buf + i, mon->response, 2);
        i += 2 + 1;


        // ������Ϣ26���ֽ�
        if ( strncmp(mon->response , "-1", 2) == 0 )
        {
            memcpy(disp_buf + i, "������", 6);
        }
        else
        {
            if ( strncmp(mon->response , "99", 2) > 0 )
            {
                if ( strncmp(mon->response , "A0", 2) == 0 )
                {
                    strcpy(disp_buf + i, "MAC У���");
                }
                else if ( strncmp(mon->response , "A1", 2) == 0 )
                {
                    strcpy(disp_buf + i, "ת�˻��Ҳ�һ��");
                }
                else if ( strncmp(mon->response , "A2", 2) == 0 )
                {
                    strcpy(disp_buf + i, "���׳ɹ������򷢿���ȷ��");
                }
                else if ( strncmp(mon->response , "A3", 2) == 0 )
                {
                    strcpy(disp_buf + i, "�ʽ������޴��˻�");
                }
                else if ( strncmp(mon->response , "A4", 2) == 0 )
                {
                    strcpy(disp_buf + i, "���׳ɹ������򷢿���ȷ��");
                }
                else if ( strncmp(mon->response , "A5", 2) == 0 )
                {
                    strcpy(disp_buf + i, "���׳ɹ������򷢿���ȷ��");
                }
                else if ( strncmp(mon->response , "A6", 2) == 0 )
                {
                    strcpy(disp_buf + i, "���׳ɹ������򷢿���ȷ��");
                }
                else if ( strncmp(mon->response , "A7", 2) == 0 )
                {
                    strcpy(disp_buf + i, "�ܾ������������쳣");
                }
                else
                {
                    strcpy(disp_buf + i, "δ֪����");
                }
            }
            else
            {
                strcpy(disp_buf + i, c_msg_text[atoi(mon->response)][1]);
            }
        }
    }


    // �����ַ�  0x00
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
            fprintf(stderr,"������ع����ڴ�ʧ��(%d) (ERRNO:%d) (%s)\n", i, errno, strerror(errno));
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
        fprintf(stderr,"�򿪼�ع����ڴ�ʧ��....\n");
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

    // ��ʼ����Ļ
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




