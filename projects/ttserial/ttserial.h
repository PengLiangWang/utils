#ifndef __TTSYS_SERIAL_H__
#define __TTSYS_SERIAL_H__

#include "ttsys.h"

__BEGIN_DECLS
/**
 * 获取流水号
 * 
 */

#define MAX_SERIAL_NO_LEN    6

int GetSerialNo(int serverId,char *name,char *logNo,int len);

__END_DECLS

#endif
