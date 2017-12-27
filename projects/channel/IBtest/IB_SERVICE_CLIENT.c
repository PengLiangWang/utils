#include "IBSERVICE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main()
{
  SERVICEBUF    askBuf,ansBuf;
  SERVICEFIELD  SBUF_SYS_RETCODE={"RETCODE",FLDTYPE_STR};
  SERVICEFIELD  SBUF_SYS_RETINFO={"RETINFO",FLDTYPE_IMG};
  SERVICEFIELD  SBUF_SYS_MESSAGE_TYPE = {"MESSAGE_TYPE",FLDTYPE_STR};
  SERVICEFIELD  SBUF_SYS_TRANS_CODE = {"TRANS_CODE",FLDTYPE_STR};
  SERVICEFIELD  SBUF_SYS_REMOTE_NODE_ID = {"REMOTE_NODE_ID",FLDTYPE_STR};
  char          retcode[3],retinfo[80];
  int           rv,size;

  memset(&askBuf,0,sizeof(askBuf));
  memset(&ansBuf,0,sizeof(ansBuf));

  rv = InitServiceBuf(&askBuf);
  if(rv != 0)
  {
    printf("InitServiceBuf error %d\n",rv);
    return rv;
  }

  rv = InitServiceBuf(&ansBuf);
  if(rv != 0)
  {
    printf("InitServiceBuf error %d\n",rv);
    FreeServiceBuf(&askBuf);
    return rv;
  }

  rv = AddServiceField(&askBuf,&SBUF_SYS_MESSAGE_TYPE,"0820",4);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  rv = AddServiceField(&askBuf,&SBUF_SYS_TRANS_CODE,"000001",6);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  rv = AddServiceField(&askBuf,&SBUF_SYS_REMOTE_NODE_ID,"3100000000",10);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  rv = AddServiceField(&askBuf,&SBUF_SYS_RETCODE,"-1",2);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  rv = AddServiceField(&askBuf,&SBUF_SYS_RETINFO,"123456",6);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }


  rv = IB_SERVICE_CALL_EX(&askBuf,&ansBuf,NULL,NULL);
  if(rv != 0)
  {
    printf("IB_SERVICE_INIT error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  memset(retcode,0,sizeof(retcode));
  memset(retinfo,0,sizeof(retinfo));

  rv = GetServiceField(&ansBuf,&SBUF_SYS_RETCODE,retcode,&size,sizeof(retcode) - 1);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  rv = GetServiceField(&ansBuf,&SBUF_SYS_RETINFO,retinfo,&size,sizeof(retinfo) - 1);
  if(rv != 0)
  {
    printf("GetServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  printf("retcode = %s,retinfo=%s\n",retcode,retinfo);

  FreeServiceBuf(&askBuf);
  FreeServiceBuf(&ansBuf);

  return 0;
  
}

