#ifndef _TM_RECORDS_INCLUDE_H_
#define _TM_RECORDS_INCLUDE_H_

#include "ISO8583.h"

typedef struct
{
     int    flag;             // 标记 0: 未使用   1: 使用 ,默认使用 0
     char   corrid[65];       // 消息标记
     int    transType;        // 交易类型
     char   logNo[7];         // 流水号
     char   amount[13];       // 交易金额
     char   mid[16];          // 商户号
     char   tid[9];           // 终端号
     char   trans_date[9];    // 交易日期
     char   trans_time[7];    // 交易时间
     char   card_no[20];      // 卡号
     char   response[3];      // 返回码  -1: 发送中
     char   response_msg[70]; // 返回信息
     //char   bankName[13];     // 银行名称
     //char   cardType[2];      // 卡类型  1: 信用卡    2: 借记卡
     long   total_time;       // 总的处理时间
}mon_data_type;

#ifndef MAX_TRANS_NUMBER
#define MAX_TRANS_NUMBER  32
#else
#undef  MAX_TRANS_NUMBER
#define MAX_TRANS_NUMBER  32
#endif

typedef struct
{
  char          change;          //change 记录当前已经显示的行数
  mon_data_type mon[MAX_TRANS_NUMBER];
}TransRecords;


int  TransRecordsInit(int server,void **shmptr);
int  TransRecordsOpen(void **shmptr);
int  TransRecordsClose(void **shmptr);
int  TransRecordsDeatory();
void TransRecordsAdd(char *corrid, ISO8583 *iso8583,TransRecords *trans);

#define BK_LOGIN                        1   /*签到*/
#define BK_LOGOUT                       2   /*签退*/
#define BK_SETTLEMENT                   3   /*结帐*/
#define BK_BACTCHUP                     4   /*批上送/批上送结束*/
#define BK_DOWNLOAD_PRM                 5   /*下装参数*/
#define BK_REECHO                       6   /*回响测试*/
#define BK_UPLOAD_STATUS                7   /*上送POS状态*/
#define BK_BALANCE                      8   /*查询余额*/
#define BK_SALE                         9   /*消费*/
#define BK_REVERSAL                    10   /*消费冲正*/
#define BK_VOID                        11   /*撤消消费*/
#define BK_VOID_REVERSAL               12   /*消费撤消冲正*/
#define BK_REFUND                      13   /*退货*/
#define BK_PREAUTH                     14   /*预授权*/
#define BK_PREAUTH_REVERSAL            15   /*预授权冲正*/
#define BK_PREAUTH_VOID                16   /*预授权撤消*/
#define BK_PREAUTH_VOID_REVERSAL       17   /*预授权撤消冲正*/
#define BK_AUTHEND                     18   /*预授权完成*/
#define BK_AUTHEND_REVERSAL            19   /*预授权完成冲正*/
#define BK_AUTHEND_VOID                20   /*预授权完成撤消*/
#define BK_AUTHEND_VOID_REVERSAL       21   /*撤消类冲正*/

#endif
