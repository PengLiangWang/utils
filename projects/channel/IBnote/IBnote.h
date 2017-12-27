#ifndef _IB_NOTE_SYS_H_
#define _IB_NOTE_SYS_H_

#define IB_TRANS_INFO         "TRANS"
#define IB_GROUPCONTROL_INFO  "GROUP"
#define IB_ADDR_INFO          "ADDR"

int IBGetMaxProcesses(int *processes);
int IBGetNiceProcessesCountStr(char *mynicegroup,char *nice,char *processes,char *count);
int IBGetNiceProcessesCountInt(char *mynicegroup,int *nice,int *processes,int *count);
int IBUpdateProcessCount(char *mynicegroup,char *nice,char *maxprocess,char *currentprocess,char flag);
int IBProcessAdd(char *mynicegroup,int *nice);
int IBProcessMove_(char *mynicegroup);
int IBProcessMove(char *mynicegroup);

int IBGetTrans(char *msgType,char *transCode,char *dataType,char *transMode,int *timeout,char *group,char *program,char *summary);
int IBGetTransMode(char *msgType,char *transCode,char *transMode);
int IBGetTransNoGroup(char *msgType,char *transCode,char *dataType,char *encType,int *timeout,char *prg,char *summary);
int IBGetTransDataType(char *msgType,char *transCode,char *dataType);
int IBGetTransTimeout(char *msgType,char *transCode,int *timeout);
int IBGetTransProgram(char *msgType,char *transCode,char *transMode,char *program,char *group);
int IBGetTransSummary(char *msgType,char *transCode,char *summary);

int IBGetAddr(char *node,char *stat,char *addr1,char *addr2,int *portNo);
int IBSetAddrStat(char *node,char *stat);


#endif
