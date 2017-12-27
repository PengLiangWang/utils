#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "datetime.h"


void GetCurrentDateTime(TDateTime *datetime)
{
  time_t       timer;
  struct tm   *lt;
 
  memset(datetime,0,sizeof(TDateTime));
  time(&timer);
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

#define DEF_TIME_FMT   "%Y/%m/%d"

void GetNextDate(const char *beginDate,char *nextDate)
{
  char  tmpBuf[11];
  struct tm  tmstu,*ptime;
  time_t timeb,timen;

  memset(tmpBuf,0,sizeof(tmpBuf));
  memset(&tmstu,0,sizeof(tmstu));

  strncpy(tmpBuf,beginDate,4);
  strcat(tmpBuf,"/");
  strncpy(tmpBuf + 4 + 1,beginDate + 4,2);
  strcat(tmpBuf,"/");
  strncpy(tmpBuf + 7 + 1,beginDate + 6,2);

  strptime( (char *)tmpBuf,(char *)DEF_TIME_FMT,(struct tm *)&tmstu);
  timeb = mktime( &tmstu ) ;
  timen = timeb + 24*60*60;

  ptime = localtime( &timen ) ;

  sprintf(nextDate,"%04d%02d%02d",ptime->tm_year + 1900,ptime->tm_mon + 1,ptime->tm_mday);

  return ;
}

void GetPrevDate(const char *beginDate,char *prevDate)
{
  char  tmpBuf[11];
  struct tm  tmstu,*ptime;
  time_t timeb,timen;

  memset(tmpBuf,0,sizeof(tmpBuf));
  memset(&tmstu,0,sizeof(tmstu));

  strncpy(tmpBuf,beginDate,4);
  strcat(tmpBuf,"/");
  strncpy(tmpBuf + 4 + 1,beginDate + 4,2);
  strcat(tmpBuf,"/");
  strncpy(tmpBuf + 7 + 1,beginDate + 6,2);

  strptime( (char *)tmpBuf,(char *)DEF_TIME_FMT,(struct tm *)&tmstu);
  timeb = mktime( &tmstu ) ;
  timen = timeb - 24*60*60;

  ptime = localtime( &timen ) ;

  sprintf(prevDate,"%04d%02d%02d",ptime->tm_year + 1900,ptime->tm_mon + 1,ptime->tm_mday);

  return ;
}
