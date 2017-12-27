#include <stdio.h>
#define LINE  512
#include "uit_read.h"

void main(void)
{
  int r=0;
  char  strBuf[LINE];

  WinInit();
  r = ChooseDispTextAreaFile(
        "ttt.txt",
/*        "/home/yan/etc/TRANS", */
        2,6,22,65,
/*        NULL,*/
        "Choose: ----[ÎÄ¼þ´°¿Ú]----",
        strBuf);
  WinEnd();
  if(r!=-1)
    printf("Your Choose:\n[Line: %d]\n[%s]\n",r,strBuf);
  else
    printf("Choose:[]. or <ESC>.\n");
}
