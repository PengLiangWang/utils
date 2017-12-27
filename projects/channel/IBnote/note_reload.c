#include "note.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cursUtil.h"
#include "logger.h"

static char	RELOAD[2];
static CursInputItem ReloadInputItem[]
=
{
  {"       是否确认重新加载系统参数(Y/N) : ",RELOAD,4,2,2,1}
};

static CursInputWindow	ReloadInputWindow
=
{
  "IBserver参数重载",
   7,50,6,17,
   1,ReloadInputItem
};

static int inputInitItem()
{
  int  r,g,init;

  memset(RELOAD,0,sizeof(RELOAD));
  CursInputWindow_open(&ReloadInputWindow);
  g = 1;
  while (g)
  {
    r = CursInputWindow_input(&ReloadInputWindow);
    switch (r)
    {
      case INPUTACTION_END:
        init = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        init = 0;
        g = 0;
        break;
    }
  }
LOG_TRACE("start close");
  CursInputWindow_close(&ReloadInputWindow);
LOG_TRACE("end   close");
  return init;
}

int main(int argc,char *argv[])
{
  int  init,g = 1;


  CursInit();
  while (g)
  {
    init = inputInitItem();
    if (init == 0)
    {
      g = 0;
      continue;
    }
    if ((strcmp(RELOAD,"y")) != 0 && (strcmp(RELOAD,"Y")) != 0 &&
        (strcmp(RELOAD,"n")) != 0 && (strcmp(RELOAD,"N")) != 0)
    {
      logger(__FILE__,__LINE__,"reload = %s",RELOAD);
      continue;
    }
    else 
      g = 0;
  }
  CursEnd();

  if(RELOAD[0] == 'Y' || RELOAD[0] == 'y')
  {
    note_queue_reload();
  }

  return 0;
}
