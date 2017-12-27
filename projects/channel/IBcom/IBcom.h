#ifndef _SYS_IB_COM_H_
#define _SYS_IB_COM_H_


#ifdef __cplusplus
extern "C"{
#endif


/*IB���ݴ������󳤶�*/
#define IB_MAXDATABLOCK          8192 + 32

/*IB�ļ���������ݿ�Ĵ�С*/
#define IB_FILE_DATA_UINT        8192

/*IB�ڵ��ŵĳ���*/
#define NODE_MAXLEN              10

/*IBͨѶ��Ϣ���͵ĳ���*/
#define MSG_TYPE_LEN             4

/*IBͨѶ���ױ���ĳ���*/
#define TRANS_CODE_LEN           6

/*IBͨѶĬ�ϳ�ʱʱ��*/
#define IB_DEFAULT_TIMEOUT       30

/*�����Ƿ��MACУ��*/
#define IB_HAS_MAC               1
#define IB_NO_MAC                0

/*���ܴ����ʶ*/
#define IB_IS_ENCRYPT            1
#define IB_NO_ENCRYPT            0

#define PACKET_IN                1
#define PACKET_OUT               2

/*�������*/
#define IB_CLIENT_IS_NOT_AUTH    1
#define IB_SERVICE_NOTE_FOUND    2
#define IB_DATA_MAC_ERROR        3


/*ͨѶ��ʱʱ��*/
extern int IB_TRANS_TIMEOUT;



typedef struct connection
{
  char   remote_node_id[NODE_MAXLEN + 1];
  char   local_node_id[NODE_MAXLEN + 1];
  char   message_type[MSG_TYPE_LEN + 1];
  char   trans_code[TRANS_CODE_LEN + 1];
  int    pkg_type;
  int    has_snd_file;
  int    has_rcv_file;
  int    sock;
}Connection;


int IBrcvData(Connection *conn,char *rcvFile,void *data,
             int (* packetFunction)(void *data,char *buf,int buflen,int *use,int in_out),...);

int IBsndData(Connection *conn,char *sndFile,void *data,
             int (* packetFunction)(void *data,char *buf,int buflen,int *use,int in_out),...);

int IBinitEnv();

int IBacceptConnect(Connection *conn);

int IBdisconnect(Connection *conn);

int IBconnect(Connection *conn,void *pubdata,
             int (*get_pub_data_f)(void *pubdata,char *node,char *tranCode,char *msgType));

#ifdef __cplusplus
}
#endif

#endif
