#ifndef _IB_SYS_NOTE_QUEUE_H_
#define _IB_SYS_NOTE_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NOTE_GET       1
#define NOTE_PUT       2
#define NOTE_RELOAD    3
#define NOTE_EXIT      4

#ifndef NOTEBUFSIZE

#define NOTEBUFSIZE   128

#else

#undef  NOTEBUFSIZE
#define NOTEBUFSIZE   128

#endif

typedef struct
{
  long  mtype;
  int   cmd;
  int   ans;
  char  index_str[NOTEBUFSIZE]; 
  char  value_str[NOTEBUFSIZE]; 
} Cmd;

int note_queue_init(int server);
int note_queue_remove();
int note_queue_cmd(Cmd *cmd);
int note_queue_recv_cmd(Cmd *cmd);
int note_queue_send_ans(Cmd *cmd);
int note_queue_get(char *index,char *value);
int note_queue_put(char *index,char *value);
int note_queue_reload();
int note_queue_exit();

#ifdef __cplusplus
}
#endif

#endif
