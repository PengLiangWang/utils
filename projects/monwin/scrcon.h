#ifndef _SCRCON_INCLUDE_H_
#define _SCRCON_INCLUDE_H_

#define RECORD_ITEM_COUNT   10
#define TRADE_TYPE_COUNT   12
enum
{
    CPS_SCR_LMAX =   80,
    CPS_SCR_HMAX =   24,
    CPS_COO_YDTY =   22,
    CPS_COO_YMDX =    2,
    CPS_COO_DATX =   14,
    CPS_COO_TMEX =   19,
    CPS_COO_AUTX =   36,
    CPS_TRC_WINY =    4,
    CPS_TRC_WINX =    3,
    CPS_TRC_WINL =   17,
    CPS_TRC_WINC =   75,
    CPS_MSG_TRAC = 1978,
};

typedef struct Scr_con
{
    WINDOW *framewin;
    int height_framewin;
    int length_framewin; 
    int ystart_framewin;
    int xstart_framewin;
    
    WINDOW *monwin; 
    int height_monwin;
    int length_monwin; 
    int ystart_monwin;
    int xstart_monwin;
    
    char *headline;
        
    int ymax;
    int xmax;
    int yheadline;
    int xheadline;
    int ymenu;
    int xmenu;
    int ydataStart;
    int xdataStart;
    int ydataEnd;
    int xdataEnd;
    int ytime;
    int xtime;
    int yauth;
    int xauth;
} ScrCon;

extern ScrCon scrCon;

void scrConInit();
int show_scrmain();
#endif

