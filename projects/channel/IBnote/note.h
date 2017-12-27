#ifndef _IB_SYS_NOTE_H_
#define _IB_SYS_NOTE_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifndef NOTEBUFSIZE

#define NOTEBUFSIZE   128

#else

#undef  NOTEBUFSIZE
#define NOTEBUFSIZE   128

#endif

void note_clean();
int  note_index(const char *value);
int  note_v_index(const char *value1,...);
int  note_get(int size,char *value);
int  note_v_get(int size1,char *value1,...);
int  note_v_put(const char *value1,...);
int  note_putstr(int *n,char *buf,const char *str);

#ifdef __cplusplus
}
#endif

#endif
