#include "monpub.h"
#include "ttsys.h"
#include "act/DB_PosJnls.h"
#include "datetime.h"
#include "scrcon.h"
ScrCon scrCon;

const char *menuItem[] = {"�������", "�̻���", "�ն˱���", "��������", "����ʱ��", 
                            "������ˮ", "���׽��", 
                            "����״̬", 
                            "���׷�����", 
                            "���ױ��"};
const int menuItemBeforeSpaceNum[] = {0, 9, 6, 1, 1,
                                        1, 4, 1, 1, 1};
const int recordItemBeforeSpaceNum[] = {0, 1, 1, 1, 1,
                                        3, 1, 6, 8, 8};
const char *auth = "AUTH: SONEYE    ��CTRL+C�˳�";

const char *tradeType[TRADE_TYPE_COUNT] = 
                        {"��ͨ����    ", 
                         "posǩ��     ", 
                         "��ѯ        ", 
                         "����        ",
                         "����        ",
                         "������      ", 
                         "ǩ��        ", 
                         "�ն˻������", 
                         "�˻�        ",
                         "���ѳ���    ",
                         "���ѳ���    ",
                         "���ѳ�������"
                         };
const char *servCode[TRADE_TYPE_COUNT] = 
                        {"100001", "100002", "200001", "200002",
                        "100003", "100004", "100005", "100006",
                        "200004", "300002", "200003", "300003" };
void scrConInit()
{
    int y, x;
    
    getmaxyx(stdscr, scrCon.ymax, scrCon.xmax);
    if (scrCon.ymax<CPS_SCR_HMAX || scrCon.xmax<CPS_SCR_LMAX)
    {
        ELOG(ERROR, "This Screen is too small [%d:%d] !\n",scrCon.ymax,scrCon.xmax);
        return;
    }
    
    y = scrCon.ymax;
    x = scrCon.xmax;
    
    scrCon.headline = "��    ���Ŵ���ƽ̨ʵʱ���ϵͳ    ��"; 
    scrCon.yheadline = 0;  //�����к�
    scrCon.xheadline = (x - strlen(scrCon.headline))/2;    //�����к�
    
    scrCon.height_framewin = y - 1;
    scrCon.length_framewin = x - 2;
    scrCon.ystart_framewin = 1;
    scrCon.xstart_framewin = 1;

    scrCon.ymenu = 2;
    scrCon.xmenu = 3;

    scrCon.ydataStart = 3;
    scrCon.xdataStart = 2;
    scrCon.ydataEnd = y - 3;
    scrCon.xdataEnd = 2;

    scrCon.ytime = y - 2;
    scrCon.xtime = 3;
    scrCon.yauth = y - 2;
    scrCon.xauth = x - strlen(auth) - 2;
    
    scrCon.height_monwin = y - 7;
    scrCon.length_monwin = x - 6;
    scrCon.ystart_monwin = 4;
    scrCon.xstart_monwin = 3;
}

int show_scrmain()
{
    char menu[1024] = {0};
    int var_i, var_j;
    char *pmenuItem = menu;

    clear();
    mvprintw(scrCon.yheadline, scrCon.xheadline, "%s", scrCon.headline);
    scrCon.framewin = subwin(stdscr, scrCon.height_framewin, scrCon.length_framewin, 
                                        scrCon.ystart_framewin, scrCon.xstart_framewin);    
    box(scrCon.framewin, '|', ACS_HLINE);  
    
    move(scrCon.ymenu, scrCon.xmenu);
    for (var_i = 0; var_i < sizeof(menuItem)/sizeof(*menuItem); var_i ++)
    {
        for (var_j = 0; var_j < menuItemBeforeSpaceNum[var_i]; var_j ++)
        {
            addch(' ');
        }
        addstr(menuItem[var_i]);
    }
    
    mvhline(scrCon.ydataStart, scrCon.xdataStart, '-', scrCon.xmax - 4);
    mvhline(scrCon.ydataEnd, scrCon.xdataEnd, '-', scrCon.xmax - 4);

    mvprintw(scrCon.yauth, scrCon.xauth, "%s", auth);
    
    scrCon.monwin = subwin(scrCon.framewin, scrCon.height_monwin, scrCon.length_monwin, 
                                                scrCon.ystart_monwin, scrCon.xstart_monwin);
    scrollok(scrCon.monwin, TRUE);
    
    refresh();        
    return 0;
}
