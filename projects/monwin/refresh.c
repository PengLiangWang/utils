#include "monpub.h"
#include "ttsys.h"
#include "act/DB_PosJnls.h"
#include "datetime.h"
#include "scrcon.h"

extern int recordItemBeforeSpaceNum[RECORD_ITEM_COUNT];
extern char *tradeType[TRADE_TYPE_COUNT];
extern char *servCode[TRADE_TYPE_COUNT];
char caEngDate[][4]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

int cps_timedisp(Disptime *psDisptime)
{
    int  iRet;
    long t;
    struct tm *tp;

    time(&t);
    tp=(struct tm *)localtime(&t);

    sprintf(psDisptime->caDay,"%04d.%02d.%02d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
    sprintf(psDisptime->caDate,"%s",caEngDate[tp->tm_wday]);
    sprintf(psDisptime->caTime,"%02d:%02d:%02d",tp->tm_hour,tp->tm_min,tp->tm_sec);

    return 0;
}

void GetXSecondsBeforeCurrentDateTime(TDateTime *datetime, int xSeconds)
{
    time_t      timer;
    struct tm   *lt;

    memset(datetime, 0, sizeof(TDateTime));
    time(&timer);
    timer -= xSeconds;

    lt = (struct  tm  *)localtime(&timer);
    datetime->y   = lt->tm_year+1900;
    datetime->mon = lt->tm_mon+1;
    datetime->d   = lt->tm_mday;
    datetime->h   = lt->tm_hour;
    datetime->min = lt->tm_min;
    datetime->s   = lt->tm_sec;
    sprintf(datetime->hms1,"%02d%02d%02d",      datetime->h,datetime->min,datetime->s);
    sprintf(datetime->hms2,"%02d:%02d:%02d",    datetime->h,datetime->min,datetime->s);
    sprintf(datetime->hms3,"%02d时%02d分%02d秒",datetime->h,datetime->min,datetime->s);
    sprintf(datetime->ymd1,"%04d%02d%02d",      datetime->y,datetime->mon,datetime->d);
    sprintf(datetime->ymd2,"%04d/%02d/%02d",    datetime->y,datetime->mon,datetime->d);
    sprintf(datetime->ymd3,"%04d年%02d月%02d日",datetime->y,datetime->mon,datetime->d);
    sprintf(datetime->mdy1,"%02d%02d%04d",      datetime->mon,datetime->d,datetime->y);
    sprintf(datetime->mdy2,"%02d/%02d/%04d",    datetime->mon,datetime->d,datetime->y);
    sprintf(datetime->time,"%04d%02d%02d%02d%02d%02d",datetime->y,datetime->mon,datetime->d,datetime->h,datetime->min,datetime->s);
    
    return ;
}

int show_sInfo_data(Select_Info *psInfo, int sInfo_has_data, int *piY)
{
    int iRet;
    char caMac[17],caInfo[80],amountStr[14];
    MsgQueMon sMonqry;
    Disptime  sDisptime;
    static PosJnls payJnls;
    
    memset(caMac,0,17);
    memset(&sDisptime,0,sizeof(Disptime));
    memset(&payJnls,0,sizeof(PosJnls));
    iTrans = 0;
    lRows  = 0;
    
    for (; ; ) 
    {
#if 1
        //int iLay;
        memset(&sMonqry,0,sizeof(MsgQueMon));
        memset(caInfo,0,sizeof(caInfo));
        iRet = msgrcv(iMsg,&sMonqry,sizeof(MsgQueMon)-sizeof(long),iTrans,IPC_NOWAIT);
        if (iRet!=-1)
        {
            strncpy(caInfo,sMonqry.info,CPS_TRC_WINC-1);
            if (lRows++>0)  
                wprintw(scrCon.monwin,"\n");
            wprintw(scrCon.monwin,"%s",caInfo);
            wrefresh(scrCon.monwin);
        }
#endif
#if 1 
        iRet = cps_timedisp(&sDisptime);
        if (iRet)  
            return iRet;
        md5_calc(caMac,(unsigned char *)&sDisptime,sizeof(Disptime)-17);
        if (memcmp(caMac,sDisptime.caDmac,16))
        {
            memcpy(sDisptime.caDmac,caMac,16);
            move(scrCon.ytime, scrCon.xtime);
            addstr(sDisptime.caDay);
            addstr("  ");
            addstr(sDisptime.caDate);
            addstr("  ");
            addstr(sDisptime.caTime);
            
            refresh();
        }
#endif   
        if (sInfo_has_data == 0)
            return 0;
              
        iRet = DB_PosJnls_fetch_select(psInfo ,&payJnls);
        if(iRet != TTS_SUCCESS && iRet != SQLNOTFOUND)
        {
            ELOG(ERROR, "Fetch 表PosJnls失败, 数据库错误码[%d]", iRet);
            return -1;

        }
        else if(iRet == SQLNOTFOUND)
        {
            DB_PosJnls_close_select(psInfo);
            return 0;
        }
            
        char amount[14] = {0};
        char *recordItem[RECORD_ITEM_COUNT] = {0};
        int var_i, var_j;
    

        //此处修改记录数据项 
        sprintf(amount, "%13.2f", payJnls.amount);
        for (var_i = 0; var_i < RECORD_ITEM_COUNT; var_i ++)
        {
            if (strncmp(payJnls.servCode, servCode[var_i], sizeof(payJnls.servCode)) == 0)
            {    
                recordItem[0] = tradeType[var_i];
                break;
            }
        }
        recordItem[1] = payJnls.merchId;
        recordItem[2] = payJnls.termId;
        recordItem[3] = payJnls.localDate;
        recordItem[4] = payJnls.localTime; 
        recordItem[5] = payJnls.localLogNo; 
        recordItem[6] = amount;
        recordItem[7] = payJnls.status;
        recordItem[8] = payJnls.msgCode; 
        recordItem[9] = payJnls.payTag; 
        //记录数据项修改完毕
        

        #if 1
        for (var_i = 0; var_i < sizeof(recordItem)/sizeof(*recordItem); var_i ++)
        {
            for (var_j = 0; var_j < recordItemBeforeSpaceNum[var_i]; var_j ++)
            {
                waddch(scrCon.monwin, ' ');
            }
            waddstr(scrCon.monwin, recordItem[var_i]);
        }
        waddch(scrCon.monwin, '\n');
        #endif
        wrefresh(scrCon.monwin);
    }
}


int cps_refresh()
{
    int iRet;
    int iMaxSelect;
    int sInfo_for_localDate_G_has_data, sInfo_for_localTime_GE_has_data;    
    int iY = CPS_TRC_WINY;
    
    TDateTime dateTimeToSelectMin, dateTimeToSelectMax, 
                dateTimeJustTmpMin, dateTimeJustTmpMax;
    Select_Info sInfo_for_localDate_G, sInfo_for_localTime_GE_L;

    BusinessDataBaseOpen();  
    
    memset(&dateTimeJustTmpMin, 0, sizeof(TDateTime));
    memset(&dateTimeJustTmpMax, 0, sizeof(TDateTime));
    GetXSecondsBeforeCurrentDateTime(&dateTimeJustTmpMax, TIME_OUT - 10);
    GetXSecondsBeforeCurrentDateTime(&dateTimeJustTmpMin, TIME_OUT);

    for (; ; )
    {
        sleep(10);        

        dateTimeToSelectMin = dateTimeJustTmpMin; 
        dateTimeToSelectMax = dateTimeJustTmpMax; 
   
        dateTimeJustTmpMin = dateTimeJustTmpMax;
 
        iMaxSelect = 0;
        memset(&sInfo_for_localDate_G, 0, sizeof(Select_Info));
        iRet = DB_PosJnls_open_select_by_localDate_G(dateTimeToSelectMin.ymd1, &sInfo_for_localDate_G);
        if (iRet != TTS_SUCCESS && iRet != SQLNOTFOUND)
        {
            ELOG(ERROR, "DB_PosJnls_open_select_by_localDate_G ERROR[%d]!\n", iRet);
            iMaxSelect ++;
            if (iMaxSelect == 100)
                break;
            continue;
        }
        else if (iRet == SQLNOTFOUND)
        {
            ELOG(ERROR, "DB_PosJnls_open_select_by_localDate_G ERROR[%d]!\n", iRet);
            sInfo_for_localDate_G_has_data = 0;  //没有数据
        } 
        else 
            sInfo_for_localDate_G_has_data = 1;  //有数据

        iMaxSelect = 0;
        memset(&sInfo_for_localTime_GE_L, 0, sizeof(Select_Info));
        iRet = DB_PosJnls_open_select_by_localDate_and_localTime_GE_and_localTime_L(dateTimeToSelectMin.ymd1, dateTimeToSelectMin.hms1, dateTimeToSelectMax.hms1, &sInfo_for_localTime_GE_L);
        if (iRet != TTS_SUCCESS && iRet != SQLNOTFOUND)
        {
            ELOG(ERROR, "DB_PosJnls_open_select_by_localDate_GE_and_localTime_L ERROR[%d]!\n", iRet);
            iMaxSelect ++;
            if (iMaxSelect == 100)
                break;
            continue;
        }
        else if (iRet == SQLNOTFOUND)
        {
            ELOG(ERROR, "DB_PosJnls_open_select_by_localDate_GE_and_localTime_L ERROR[%d]!\n", iRet);
            sInfo_for_localTime_GE_has_data = 0;  //没有数据
        } 
        else 
            sInfo_for_localTime_GE_has_data = 1;  //有数据
    
        /*显示数据 sInfo, sInfo_has_data, &iY */ 
        show_sInfo_data(&sInfo_for_localDate_G, sInfo_for_localDate_G_has_data, &iY);
        show_sInfo_data(&sInfo_for_localTime_GE_L, sInfo_for_localTime_GE_has_data, &iY);
        
        memset(&dateTimeJustTmpMax, 0, sizeof(TDateTime));
        GetXSecondsBeforeCurrentDateTime(&dateTimeJustTmpMax, TIME_OUT - 10);
    }
	
    BusinessDataBaseClose();
}

