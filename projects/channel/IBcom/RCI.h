#ifndef _IB_RCI_H_
#define _IB_RCI_H_

typedef struct _RCI_ENV
{
  char   message_type[5];
  char   trans_code[7];
  char   local_node_id[11];
  char   remote_node_id[11];
}RCI_ENV;

int IB_RCI_PKG_ENV(void *v_data, char *buf, \
                   int buflen, int *useP, int in_out);

int IB_RCI_INIT(RCI_ENV *rcienv,void *rcvData,\
              int (* packetFunction)\
              (void *data,char *buf,int buflen,int *use,int in_out),\
              char *rcvFile);

int IB_RCI_RETURN(RCI_ENV *rcienv,void *sndData,\
                  int (* packetFunction)\
                  (void *data,char *buf,int buflen,int *use,int in_out),\
                  char *sndFile);

int IB_RCI_CALL(RCI_ENV *rci,void *sndData,void *rcvData,\
                int (* packetInFunction)\
                  (void *data,char *buf,int buflen,int *use,int in_out),\
                int (* packetOutFunction)\
                  (void *data,char *buf,int buflen,int *use,int in_out),\
                char *sndFile, char *rcvFile);

#endif
