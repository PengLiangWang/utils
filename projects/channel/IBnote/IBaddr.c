#include <stdio.h>
#include <netdb.h>
#include "IBnote.h"
#include "note.h"
#include "note_error.h"
#include <errno.h>

extern int errno;

int IBGetAddr(char *node,char *stat,char *addr1,char *addr2,int *portNo)
{
  char   portName[32];
  int    rv;

  rv = note_v_index(IB_ADDR_INFO,node,NULL);
  if(rv != 0)
  {
    return rv;
  }

  rv = note_v_get(2,stat,31,addr1,31,addr2,31,portName,0);
  if(rv != 0 && rv != NOTE_ENDOFDATA)
  {
    logger(__FILE__,__LINE__,"Get [%s][%s] AddrList Err[%d]",IB_ADDR_INFO,node,rv);
    return rv;
  }

  *portNo = atoi(portName);

  if(addr1[0] == '\0' && addr2[0] == '\0')
    return -1;  
    
  if (*stat == 'B')
  {
    char tempstr[32];
    strcpy(tempstr,addr1);
    strcpy(addr1,addr2);
    strcpy(addr2,tempstr);  
  }

  return 0;
}

int IBSetAddrStat(char *node,char *stat)
{
  int     rv;
  char    stat_[4],addr1[32],addr2[32],portName[32];

  rv = note_v_index(IB_ADDR_INFO,node,NULL);
  if(rv)
  {
    return rv;
  }

  rv = note_v_get(2,stat_,31,addr1,31,addr2,31,portName,0);
  if(rv != 0 && rv != NOTE_ENDOFDATA)
  {
    logger(__FILE__,__LINE__,"Get [%s] AddrList Err[%d]",node,rv);
    return rv;
  }

  rv = note_v_put(stat,addr1,addr2,portName,NULL);
  if(rv != 0)
  {
    logger(__FILE__,__LINE__,"Put [%s] AddrList Err[%d]",node,rv);
    return rv;
  }

  return 0;
}
