#ifndef __TTSYS_SERIAL_H__
#define __TTSYS_SERIAL_H__

#include "ttsys.h"

__BEGIN_DECLS
/**
 * ��ȡ��ˮ��
 * 
 */

#define MAX_SERIAL_NO_LEN    6

int GetSerialNo(int serverId,char *name,char *logNo,int len);

__END_DECLS

#endif
