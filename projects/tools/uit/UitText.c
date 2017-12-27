#include <stdio.h>
#include <string.h>
#include "UIT.h"

int UitPrintWinText(PWIN *win,int top,int left,char **text)
{
  int i;

  if (top>0)
    top--;
  if (text==NULL)
    return(0);
  for(i=0; text[i]!=NULL; ++i)
    WinDrawString(win,text[i],top+i,left,A_NORMAL);
  return(0);
}

int UitPrintWinText_B(UitFormCtrl *form, char *b,int top,int left,char **text)
{
  int i;

  if (top>0)
    top--;
  if (text==NULL)
    return(0);
  for(i=0; text[i]!=NULL; ++i)
    WinDrawString_B(&form->win, b,text[i],top+i,left);
  return(0);
}
