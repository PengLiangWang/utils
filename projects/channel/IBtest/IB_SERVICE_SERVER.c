#include "IBSERVICE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  SERVICEBUF  askBuf,ansBuf;
  SERVICEFIELD SBUF_SYS_RETCODE={"RETCODE",FLDTYPE_STR};
  SERVICEFIELD SBUF_SYS_RETINFO={"RETINFO",FLDTYPE_IMG};
  SERVICEFIELD SBUF_SYS_AMOUNT ={"AMOUNT",FLDTYPE_DOUBLE};
  SERVICEFIELD SBUF_SYS_YEAR   ={"YEAR",FLDTYPE_INT};
  int  rv;
  double amt = 1289.987;
  int    year = 2000;

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

  rv = IB_SERVICE_INIT_EX(&askBuf,NULL);
  if(rv != 0)
  {
    printf("IB_SERVICE_INIT error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }
  
  ServiceBufDebug("SYSBUF","请求数据",&askBuf);

  rv = AddServiceField(&ansBuf,&SBUF_SYS_RETCODE,"00",2);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    IB_SERVICE_ABORT();
    return rv;
  }

  rv = AddServiceField(&ansBuf,&SBUF_SYS_RETINFO,"交易成功",8);
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    IB_SERVICE_ABORT();
    return rv;
  }

  rv = AddServiceField(&ansBuf,&SBUF_SYS_AMOUNT,(char*)&amt,sizeof(amt));
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    IB_SERVICE_ABORT();
    return rv;
  }

  rv = AddServiceField(&ansBuf,&SBUF_SYS_YEAR  ,(char*)&year,sizeof(year));
  if(rv != 0)
  {
    printf("AddServiceField error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    IB_SERVICE_ABORT();
    return rv;
  }

  rv = IB_SERVICE_RETURN_EX(&ansBuf,NULL);
  if(rv != 0)
  {
    printf("IB_SERVICE_RETURN error %d\n",rv);
    FreeServiceBuf(&askBuf);
    FreeServiceBuf(&ansBuf);
    return rv;
  }

  FreeServiceBuf(&askBuf);
  FreeServiceBuf(&ansBuf);

  return 0;
  
}

