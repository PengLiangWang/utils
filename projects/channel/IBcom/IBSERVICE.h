#ifndef _IB_SBUF_H_
#define _IB_SBUF_H_

#include "ServiceBuf.h"

#ifdef __cplusplus
extern "C" {
#endif

int IB_SERVICE_INIT(SERVICEBUF *sBuf,char *rcvFile);
int IB_SERVICE_RETURN(SERVICEBUF *sBuf,char *sndFile);
int IB_SERVICE_ABORT();
int IB_SERVICE_CALL(SERVICEBUF *sndBuf,SERVICEBUF *rcvBuf,char *sndFile,char *rcvFile);

int IB_SERVICE_INIT_EX(SERVICEBUF *sBuf,char *rcvFile);
int IB_SERVICE_RETURN_EX(SERVICEBUF *sBuf,char *sndFile);
int IB_SERVICE_CALL_EX(SERVICEBUF *sndBuf,SERVICEBUF *rcvBuf,char *sndFile,char *rcvFile);

#ifdef __cplusplus
}
#endif

#endif
