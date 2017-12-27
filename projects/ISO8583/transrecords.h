#ifndef _TM_RECORDS_INCLUDE_H_
#define _TM_RECORDS_INCLUDE_H_

#include "ISO8583.h"

typedef struct
{
     int    flag;             // ��� 0: δʹ��   1: ʹ�� ,Ĭ��ʹ�� 0
     char   corrid[65];       // ��Ϣ���
     int    transType;        // ��������
     char   logNo[7];         // ��ˮ��
     char   amount[13];       // ���׽��
     char   mid[16];          // �̻���
     char   tid[9];           // �ն˺�
     char   trans_date[9];    // ��������
     char   trans_time[7];    // ����ʱ��
     char   card_no[20];      // ����
     char   response[3];      // ������  -1: ������
     char   response_msg[70]; // ������Ϣ
     //char   bankName[13];     // ��������
     //char   cardType[2];      // ������  1: ���ÿ�    2: ��ǿ�
     long   total_time;       // �ܵĴ���ʱ��
}mon_data_type;

#ifndef MAX_TRANS_NUMBER
#define MAX_TRANS_NUMBER  32
#else
#undef  MAX_TRANS_NUMBER
#define MAX_TRANS_NUMBER  32
#endif

typedef struct
{
  char          change;          //change ��¼��ǰ�Ѿ���ʾ������
  mon_data_type mon[MAX_TRANS_NUMBER];
}TransRecords;


int  TransRecordsInit(int server,void **shmptr);
int  TransRecordsOpen(void **shmptr);
int  TransRecordsClose(void **shmptr);
int  TransRecordsDeatory();
void TransRecordsAdd(char *corrid, ISO8583 *iso8583,TransRecords *trans);

#define BK_LOGIN                        1   /*ǩ��*/
#define BK_LOGOUT                       2   /*ǩ��*/
#define BK_SETTLEMENT                   3   /*����*/
#define BK_BACTCHUP                     4   /*������/�����ͽ���*/
#define BK_DOWNLOAD_PRM                 5   /*��װ����*/
#define BK_REECHO                       6   /*�������*/
#define BK_UPLOAD_STATUS                7   /*����POS״̬*/
#define BK_BALANCE                      8   /*��ѯ���*/
#define BK_SALE                         9   /*����*/
#define BK_REVERSAL                    10   /*���ѳ���*/
#define BK_VOID                        11   /*��������*/
#define BK_VOID_REVERSAL               12   /*���ѳ�������*/
#define BK_REFUND                      13   /*�˻�*/
#define BK_PREAUTH                     14   /*Ԥ��Ȩ*/
#define BK_PREAUTH_REVERSAL            15   /*Ԥ��Ȩ����*/
#define BK_PREAUTH_VOID                16   /*Ԥ��Ȩ����*/
#define BK_PREAUTH_VOID_REVERSAL       17   /*Ԥ��Ȩ��������*/
#define BK_AUTHEND                     18   /*Ԥ��Ȩ���*/
#define BK_AUTHEND_REVERSAL            19   /*Ԥ��Ȩ��ɳ���*/
#define BK_AUTHEND_VOID                20   /*Ԥ��Ȩ��ɳ���*/
#define BK_AUTHEND_VOID_REVERSAL       21   /*���������*/

#endif
