#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/timeb.h>
#include <time.h>
#include "transrecords.h"

static int ipckey = -1;
static int shmid = -1;

static int get_key(char *file,int *key)
{
    char  filename[128];
    key_t result;

    memset(filename,0,sizeof(filename));
    sprintf(filename,"%s/etc/%s",getenv("HOME"),file);
    result = ftok(filename,1);
    if (result == -1)
    {
        return -1;
    }
    *key = (int)result;

    return 0;

}

int TransRecordsInit(int server,void **shmptr)
{
    int    size = 0;
    extern int errno;

    if(get_key("TransRecords",&ipckey))
    {
        return -2;
    }

    size = sizeof(TransRecords);
    shmid = shmget(ipckey,size ,0660 | ( server ? IPC_CREAT : 0 ));
    if (shmid == -1)
    {
        return -3;
    }

    *shmptr = (char *)shmat(shmid,0,0);
    if( *shmptr == NULL )
    {
        return -4;
    }

    if (server)
    {
        memset(*shmptr,0,size);
    }

    return 0;

}

int TransRecordsOpen(void **shmptr)
{
    int    size = 0;
    extern int errno;

    if(get_key("TransRecords",&ipckey))
    {
        return -2;
    }

    size = sizeof(TransRecords);
    shmid = shmget(ipckey,size ,0660);
    if (shmid == -1)
    {
        return -3;
    }

    *shmptr = (char *)shmat(shmid,0,0);
    if( *shmptr == NULL )
    {
        return -4;
    }

    return 0;
}

int TransRecordsClose(void **shmptr)
{
    return shmdt(*shmptr);
}

int TransRecordsDeatory()
{
    if(shmid >= 0)
    {
        if (shmctl(shmid,IPC_RMID,0) != 0)
        {
            return -1;
        }
        shmid = -1;
    }
    return 0;
}

void TransRecordsAdd(char *corrid, ISO8583 *isomsg, TransRecords *trans)
{
    mon_data_type   d;
    int             i;
    struct timeb    stime;
    int             transType,transDir;

    ftime(&stime);
    memset(&d, 0, sizeof(d) );

    i = TransRecordsGetTransType(isomsg, &transType, &transDir);
    if ( i )
    {
        printf("...error...\n");
        return ;
    }

    d.flag = 2;
    strncpy(d.corrid, corrid, 64);

    if ( !transDir )
    {
        d.transType = transType;
    }
    strncpy(d.logNo, isomsg->system_trace_audit_number, 6);
    if ( check_bitmap(isomsg, POS_BIT_amount_of_transaction) == 0 )
    {
        strncpy(d.amount, "000000000000", 12);
    }
    else
    {
        strncpy(d.amount, isomsg->amount_of_transaction, 12);
    }
    strncpy(d.mid, isomsg->card_acceptor_id_code, 15);
    strncpy(d.tid, isomsg->card_acceptor_terminal_id, 8);

    TransRecordsGetCardNo(isomsg, transType, transDir, d.card_no);

    TransRecordsGetResponse(isomsg, d.response, d.response_msg, transDir);

    TransRecordsGetTransTime(isomsg, d.trans_date, d.trans_time);

    if ( !transDir )
    {
        d.total_time = stime.time * 1000 + stime.millitm;
    }

    if ( transDir )
    {
        for (i = 0; i < MAX_TRANS_NUMBER; i++ )
        {
            if ( trans->mon[i].flag == 2 && memcmp(trans->mon[i].corrid, d.corrid, 64) == 0 )
            {
                trans->mon[i].flag = 1;
                //更新卡号、金额、返回码
                trans->mon[i].total_time = stime.time*1000 + stime.millitm - trans->mon[i].total_time;
                memcpy(trans->mon[i].card_no, d.card_no, sizeof(d.card_no));
                memcpy(trans->mon[i].logNo, d.logNo, sizeof(d.logNo));
                memcpy(trans->mon[i].amount, d.amount, sizeof(d.amount));
                memcpy(trans->mon[i].trans_time, d.trans_time, sizeof(d.trans_time));
                memcpy(trans->mon[i].trans_date, d.trans_date, sizeof(d.trans_date));
                memcpy(trans->mon[i].response, d.response, sizeof(d.response));
                memcpy(trans->mon[i].response_msg, d.response_msg, sizeof(d.response_msg));
                memcpy(trans->mon[i].mid, d.mid, sizeof(d.mid));
                memcpy(trans->mon[i].tid, d.tid, sizeof(d.tid));
                trans->change++ ;
                if (trans->change > 30000000)
                {
                    trans->change = 1 ;
                }
                break;
            }
        }
        return ;
    }

    memmove(&trans->mon[0], &trans->mon[1],sizeof(mon_data_type)*(MAX_TRANS_NUMBER - 1)) ;
    memcpy(&trans->mon[MAX_TRANS_NUMBER-1], &d, sizeof(mon_data_type)) ;
    trans->change++ ;

    if (trans->change > 30000000)
    {
        trans->change = 1 ;
    }

  return ;
}


int TransRecordsGetTransType(ISO8583 *isomsg, int *transType,int *transDir)
{
    char      condition_code[3];
    char      message_type[5];
    char      process_code[7];
    char      trans_type_code[3];
    char      network_code[4];

    *transType = -1;
    *transDir = 0;

    memset(condition_code,0,sizeof(condition_code));
    memset(trans_type_code,0,sizeof(trans_type_code));
    memset(network_code,0,sizeof(network_code));
    memset(message_type,0,sizeof(message_type));
    memset(process_code,0,sizeof(process_code));

    memcpy(message_type,isomsg->message_type,4);
    memcpy(process_code,isomsg->processing_code,6);
    memcpy(condition_code,isomsg->point_of_service_condition_code,2);
    memcpy(trans_type_code,isomsg->reserved_private_data_60,2);
    memcpy(network_code,isomsg->reserved_private_data_60 + 8,3);

    if ( message_type[2] == '1' || message_type[2] == '3')
    {
        *transDir = 1;
        return 0;
    }

    if (strcmp(message_type,"0800") == 0)
    {
        if (strcmp(network_code,"001") == 0 || strcmp(network_code,"003") || strcmp(network_code,"004"))
        {
            *transType = BK_LOGIN;
            return 0;
        }
        else if (strcmp(network_code,"360") == 0 || strcmp(network_code,"361") == 0)
        {
            *transType = BK_DOWNLOAD_PRM;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (strcmp((char*)message_type,"0820") == 0)
    {
        if (strcmp(network_code,"002") == 0)
        {
            *transType = BK_LOGOUT;
            return 0;
        }
        else if (strcmp(network_code,"301") == 0)
        {
            *transType = BK_REECHO;
            return 0;
        }
        else if (strcmp(network_code,"362") == 0)
        {
            *transType = BK_UPLOAD_STATUS;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (strcmp((char *)message_type,"0500") == 0)
    {
        if (strcmp(network_code,"201") == 0)
        {
            *transType = BK_SETTLEMENT;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (strcmp((char*)message_type,"0320") == 0)
    {
        if (strcmp(network_code,"201") == 0 || strcmp(network_code,"207") == 0 || strcmp(network_code,"206") == 0)
        {
            *transType = BK_BACTCHUP;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (strcmp((char*)message_type,"0200") == 0)
    {
        if (strcmp(process_code,"310000") == 0 && strcmp(trans_type_code,"01") == 0)
        {
            *transType = BK_BALANCE;
            return 0;
        }
        else if (strcmp(process_code,"000000") == 0 && strcmp(trans_type_code,"22") == 0)
        {
            *transType = BK_SALE;
            return 0;
        }
        else if (strcmp(process_code,"200000") == 0 && strcmp(trans_type_code,"23") == 0)
        {
            *transType = BK_VOID;
            return 0;
        }
        else if (strcmp(process_code,"000000") == 0 && strcmp(trans_type_code,"20") == 0)
        {
            *transType = BK_AUTHEND;
            return 0;
        }
        else if (strcmp(process_code,"200000") == 0 && strcmp(trans_type_code,"21") == 0)
        {
            *transType = BK_AUTHEND_VOID;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (strcmp(message_type,"0400") == 0)
    {
        if (strcmp(process_code,"000000") == 0 && strcmp(trans_type_code,"22") == 0)
        {
            *transType = BK_REVERSAL;
            return 0;
        }
        else if (strcmp(process_code,"200000") == 0 && strcmp(trans_type_code,"23") == 0)
        {
            *transType = BK_VOID_REVERSAL;
            return 0;
        }
        else if (strcmp(process_code,"030000") == 0 && strcmp(trans_type_code,"10") == 0)
        {
            *transType = BK_PREAUTH_REVERSAL;
            return 0;
        }
        else if (strcmp(process_code,"200000") == 0 && strcmp(trans_type_code,"11") == 0)
        {
            *transType = BK_PREAUTH_VOID_REVERSAL;
            return 0;
        }
        else if (strcmp(process_code,"000000") == 0 && strcmp(trans_type_code,"20") == 0)
        {
            *transType = BK_AUTHEND_REVERSAL;
            return 0;
        }
        else if (strcmp(process_code,"200000") == 0 && strcmp(trans_type_code,"21") == 0)
        {
            *transType = BK_AUTHEND_VOID_REVERSAL;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (strcmp(message_type,"0100") == 0)
    {
        if (strcmp(process_code,"030000") == 0 && strcmp(trans_type_code,"10") == 0)
        {
            *transType = BK_PREAUTH;
            return 0;
        }
        else if (strcmp(process_code,"200000")==0 && strcmp(trans_type_code,"11")==0)
        {
            *transType = BK_PREAUTH_VOID;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (strcmp(message_type,"0220") == 0)
    {
        if (strcmp(process_code,"200000") == 0 && strcmp(trans_type_code,"25") == 0)
        {
            *transType = BK_REFUND;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}


int TransRecordsGetCardNo(ISO8583 *isomsg, int transType, int transDir, char *cardNo)
{
    // 交易类型 1 到 7之间为管理型交易
    if ( transType < 8 && transDir == 0 )
    {
        strncpy(cardNo, "0000000000000000000", 19);
        return 0;
    }

    if ( check_bitmap(isomsg, POS_BIT_primary_account_number) && transDir )
    {
        strncpy(cardNo, isomsg->primary_account_number, 19);
        return 0;
    }

    if ( check_bitmap(isomsg, POS_BIT_track_2_data) )
    {
        char  *psTemp ;
        char   caTrack2[38];
        memset(caTrack2, 0, sizeof(caTrack2));
        strncpy(caTrack2, isomsg->track_2_data,37);
        if ( ( psTemp = strstr(caTrack2, "D") ) != NULL  || (psTemp = strstr(caTrack2, "=")) != NULL)
        {
            *psTemp = '\0';
            strncpy(cardNo, caTrack2, strlen(caTrack2));
        }
        else
        {
            strncpy(cardNo, "0000000000000000000", 19);
        }
    }
    else
    {
        strncpy(cardNo, "0000000000000000000", 19);
    }

    return 0;
}

int TransRecordsGetTransTime(ISO8583 *isomsg, char *caDate,char *caTime)
{
    time_t       timer;
    struct tm   *lt;

    time(&timer);
    lt = (struct  tm  *)localtime(&timer);

    if ( check_bitmap(isomsg, POS_BIT_time_of_local_transaction) )
    {
        strncpy(caTime, isomsg->time_of_local_transaction, 6);
    }
    else
    {
        sprintf(caTime, "%02d%02d%02d", lt->tm_hour, lt->tm_min, lt->tm_sec);
    }

    if ( check_bitmap(isomsg, POS_BIT_date_of_local_transaction) )
    {
        strncpy(caDate, isomsg->date_of_local_transaction, 4);
    }
    else
    {
        sprintf(caDate, "%02d%02d", lt->tm_mon + 1, lt->tm_mday);
    }


    return 0;
}

int TransRecordsGetResponse(ISO8583 *isomsg, char *msgCode,char *msgText, int transDir)
{
    if ( transDir && check_bitmap(isomsg, POS_BIT_response_code) )
    {
        strncpy(msgCode, isomsg->response_code, 2);
        if ( strcmp(msgCode, "00") == 0 )
        {
            strncpy(msgText, "交易承兑", 8);
        }
        else
        {
            strncpy(msgText, "        ", 8);
        }
    }
    else
    {
        strncpy(msgCode, "-1", 2);
        strncpy(msgText, "处理中", 6);
    }

    return 0;
}
