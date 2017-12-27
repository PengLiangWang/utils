#include "monpub.h"
#include "ttsys.h"
#include "act/DB_PosJnls.h"
#include "datetime.h"
#include "scrcon.h"

void Trims(char *pcChar,char caSou,char caObj,int iLong)
{
    int i;
    for(i=0;i<iLong;i++)
    {
        if (*pcChar==caSou) *pcChar=caObj;
        pcChar++;
    }
}


int Read_config(const char *pcFile,const char *pcHead,char *pcName,char *pcInfo)
{
    int iRF = 0;

    char caName[64];
    char caInfo[64];
    char caRow[256];
    FILE *fp;
    struct stat sStat;

    if (lstat(pcFile,&sStat) < 0)  return -1;

    if (!S_ISREG(sStat.st_mode))  return -1;

    fp = fopen(pcFile,"r");
    if (fp == NULL)
    {
        return -1;
    }

    while(1)
    {
        if (feof(fp))
        {
            fclose(fp);
            return -2;
        }
        memset(caRow,0,sizeof(caRow));
        memset(caName,0,sizeof(caName));
        memset(caInfo,0,sizeof(caInfo));
        fgets(caRow,sizeof(caRow),fp);
        Trims(caRow,'\n',0,strlen(caRow));
        Trims(caRow,0x23,0,strlen(caRow));
        if (!strlen(caRow))  continue;
        if (!iRF)
        {
            if (memcmp(caRow,pcHead,strlen(pcHead)))  continue;
            iRF = 1;
            continue;
        }

        Trims(caRow,0x3D,0x20,strlen(caRow));
        sscanf(caRow,"%s%s",&caName,&caInfo);
        if (caName[0]==0x5B)
        {
            fclose(fp);
            return -2;
        }
        if (strcmp(caName,pcName) == 0)
        {
            strcpy(pcInfo,caInfo);
            break;
        }
    }

    fclose(fp);
    return 0;
}


int cps_scrfile(char *pcCon,int iSleep)
{
    int iRet,iY=0;
    char caFile[20],caPath[50];
    char caConfile[128];
    char caRow[1024];

    FILE *fp;

    memset(caFile,0,20);
    memset(caPath,0,50);
    memset(caConfile,0,sizeof(caConfile));
    sprintf(caConfile,"%s/etc/%s",getenv("HOME"),MON_CON_FILE);
    iRet = Read_config(caConfile,MON_CON_HEAD,pcCon,caFile);
    if (iRet)
    {
        printw("Not found %s from config !",pcCon);
        return -1;
    }

    sprintf(caPath,"%s/etc/%s",getenv("HOME"),caFile);
    if ((fp = fopen(caPath,"r")) == NULL)
    {
    //    GClog_print(__FILE__,__LINE__,"Can not open %s file !",pcCon);
        printw("Can not open %s file !",pcCon);
        return -1;
    }

    while (!feof(fp))
    {
        memset(caRow,0,sizeof(caRow));
        fgets(caRow,100,fp);
        mvprintw(iY++,0,"%s",caRow);
    }

    refresh();
    fclose(fp);
    if (iSleep>0) sleep(iSleep);
    return 0;
}


int cps_welmon()
{
    int iRet;

    if ((iMsg=msgget(CPS_MSG_TRAC,IPC_CREAT|0666))<0)
    {
        ELOG(ERROR, "Fail to create or get queue !\n");
        return -1;
    }

    iRet = cps_scrfile("SCRWELCOM",2);
    if (iRet) 
        return -2;
#if 0
    iRet = cps_scrfile("SCRMAIN",0);
    if (iRet) 
        return -2;
#endif
    iRet = show_scrmain();
    if (iRet) 
        return -2;
    return 0;
}

