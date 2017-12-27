#ifndef _UIT_EDIT_
#define _UIT_EDIT_

typedef int UF_Button;

#define BUTTON_OFF    0
#define BUTTON_ON   1

int UitFormEditStr(PWIN *win,int row,int col,int displaysize,
      int echo,int numStr,int size,char *str);
int UitFormEditDouble(PWIN *win,int row,int col,int displaysize,
      int size,int dec,double *value);
int UitFormEditDateStr(PWIN *win,int row,int col,int notnull,char *str);
int UitFormEditTimeStr(PWIN *win,int row,int col,int notnull,char *str);
int UitFormEditNum(PWIN *win,int row,int col,int displaysize,
      int sign,int size,char *value);
int UitFormEditButton(PWIN *win,int row,int col,char *title,UF_Button *value);

#endif
