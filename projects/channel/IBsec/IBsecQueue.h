#ifndef _SYS_IB_SEC_QUEUE_H_
#define _SYS_IB_SEC_QUEUE_H_


#include "IBsec.h"

int sec_queue_init(int server);
int sec_queue_remove();
int sec_queue_lock();

int sec_queue_send_cmd(IBCMDINFO *cmd);
int sec_queue_recv_ans(IBCMDINFO *cmd);

int sec_queue_recv_cmd(IBCMDINFO *cmd);
int sec_queue_send_ans(IBCMDINFO *cmd);

int sec_queue_cmd(IBCMDINFO *cmd);


#endif



