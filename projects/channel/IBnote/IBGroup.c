#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "note.h"
#include "IBnote.h"
#include "note_error.h"

#define DEFAULT_IB_MAX_PROCESSES   312
#define NORMAL_NICE_GROUP          "0"

int IBGetMaxProcesses(int *processes)
{
  int   r,max;
  char  str[10],str2[10],str3[10];

  r = note_v_index(IB_GROUPCONTROL_INFO,NORMAL_NICE_GROUP,NULL);
  if(r)
  {
    logger(__FILE__,__LINE__,"Index error r=%d",r);
    return r;
  }

  r = note_v_get(10,str,10,str2,10,str3,0);
  if( r == 0 )
  {
    *processes = atoi(str2);
  }
  else if( r == NOTE_ENDOFDATA || r == NOTE_NOTFOUND ) 
  {
    r = 0;
    *processes = DEFAULT_IB_MAX_PROCESSES;
  }
  else
  {
    logger(__FILE__,__LINE__, "Get [%s][%s] GROUPCONTROL Err[%d]",r);
    return r;
  }

  return r;
}

int IBGetNiceProcessesCountStr(char *mynicegroup,char *nice,char *processes,char *count)
{
  int  r;

  r = note_v_index(IB_GROUPCONTROL_INFO,mynicegroup,NULL);
  if(r)
  {
    logger(__FILE__,__LINE__,"Index error r=%d",r);
    return r;
  }
  r = note_v_get(10,nice,10,processes,10,count,0);
  if(r != 0 && r != NOTE_ENDOFDATA)
  {
    logger(__FILE__,__LINE__,"Get [%s] GROUPCONTROL Err[%d]",mynicegroup,r);
    return r;
  }
  return 0;
}
int IBGetNiceProcessesCountInt(char *mynicegroup,int *nice,int *processes,int *count)
{
  int   r;
  char  nice_s[10],processes_s[10],count_s[10];

  r = IBGetNiceProcessesCountStr(mynicegroup,nice_s,processes_s,count_s);
  if(r)  
    return r;

  *nice=atoi(nice_s);
  *processes=atoi(processes_s);
  *count=atoi(count_s);

  return 0;
}

int IBUpdateProcessCount(char *mynicegroup,char *nice,char *maxprocess,
    char *currentprocess,char flag)
{
  int   r,tmp;
  char  tmpstr[10];
  
  tmp = atoi(currentprocess);
  (flag=='I') ? (++tmp):(--tmp);
  if(tmp<0) 
    return -1;
  sprintf(tmpstr,"%d",tmp);
  r = note_v_index(IB_GROUPCONTROL_INFO,mynicegroup,NULL);
  r = note_v_put(nice,maxprocess,tmpstr,NULL);
  if(r)
  {
    logger(__FILE__,__LINE__, "put [%s] GROUPCONTROL Err[%d]",mynicegroup,r);
    return r;
  }
  return 0;
}

int IBProcessAdd(char *mynicegroup,int *nice)
{
  int   r;
  char  nice_s[10],max_count_s[10],count_s[10];  
  char  nice_s_2[10],max_count_s_2[10],count_s_2[10];  
  int   tmp;

  int   nicetmp,max_count,count;
  int   nicetmp_2,max_count_2,count_2;

  char  currentTotalCount[10],maxProcesses[10];
  char  normalNiceGroup[10];
  
  r = IBGetNiceProcessesCountStr(mynicegroup,nice_s,max_count_s,count_s);
  if(r)  
  {
    return r;
  }
 
  if(!strcmp(max_count_s,count_s))
  {
    logger(__FILE__,__LINE__,"group=[%s]maxCnt=[%s]CurCnt=[%s]", mynicegroup,max_count_s,count_s);
    return -1;
  }

  r = IBUpdateProcessCount(mynicegroup,nice_s,max_count_s,count_s,'I');
  if(r)  
    return r;
      
  return 0;
}

int IBProcessMove_(char *mynicegroup)
{
  int   r;
  char  nice_s[10],max_count_s[10],count_s[10];  
  int   nice,max_count,count;

  r = note_v_index(IB_GROUPCONTROL_INFO,mynicegroup,NULL);
  
  r = note_v_get(9,nice_s,9,max_count_s,9,count_s,0);
  if(r == NOTE_NOTFOUND) 
  {
    logger(__FILE__,__LINE__,"flow control not found");
    return 0;
  }
  else if(r!=0&&r!=NOTE_ENDOFDATA)
  {
    logger(__FILE__,__LINE__,"Get [%s] GROUPCONTROL Err[%d]",mynicegroup,r);
    return r;
  }
  count = atoi(count_s);    
  max_count = atoi(max_count_s);
  if(count>0)
    count--;  
  else 
  {
    logger(__FILE__,__LINE__, "nice=[%s]maxCnt=[%d]CurCnt=[%d]", nice_s,max_count,count);
    return -1;
  }

  sprintf(count_s,"%d",count);
  r = note_v_put(nice_s,max_count_s,count_s,NULL);
  if(r)
  {
    logger(__FILE__,__LINE__,"put [%s] GROUPCONTROL Err[%d]",mynicegroup,r);
    return r;
  }

  return r;      

}

int IBProcessMove(char *mynicegroup)
{
  int  r,tmp;

  r = IBProcessMove_(mynicegroup);
  if(r)  
  {
    return r;
  }
  return r;    
}
