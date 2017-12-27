#include "ttsys.h"
#include "cursUtil.h"

static char     masterKeyA1[17];
static char     masterKeyB1[17];
static char     masterKeyA2[17];
static char     masterKeyB2[17];


static  char  localNODE[11];

static CursInputItem  IBsecInitInputItem[] =
{
  {"本地节点编码    : ",localNODE,    4,2,11,1},
  {"主启动密钥 A P1 : ",masterKeyA1,  6,2,17,0},
  {"主启动密钥 A P2 : ",masterKeyA2,  8,2,17,0},
  {"主启动密钥 B P1 : ",masterKeyB1,  10,2,17,0},
  {"主启动密钥 B P2 : ",masterKeyB2,  12,2,17,0}
};

static CursInputWindow  IBsecInitInputWindow =
{
    "密钥数据初始化",
    15,50,5,10,
    5,IBsecInitInputItem
};

static int inputInitItem()
{
  int   r;
  int   g;
  int   init=0;

  strncpy(localNODE,"A000000001",10);

  memset(masterKeyA1,0,17);
  memset(masterKeyA2,0,17);
  memset(masterKeyB1,0,17);
  memset(masterKeyB2,0,17);

  IBsecInitInputItem[1].n=0;
  IBsecInitInputItem[2].n=0;
  IBsecInitInputItem[3].n=0;
  IBsecInitInputItem[4].n=0;
  
  CursInputWindow_open(&IBsecInitInputWindow);
  g = 1;
  while(g)
  {
    r = CursInputWindow_input(&IBsecInitInputWindow);
    switch(r)
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
  CursInputWindow_close(&IBsecInitInputWindow);
 
  return init;
}

static CursMsgWindow  msgWindow
  = {      "信    息    窗", 15,40,2,10      };

static int displayMsg(char *msg)
{
  int key;

  CursMsgWindow_open(&msgWindow);
  CursMsgWindow_printw(&msgWindow,"\n\n%s\n",msg);
  key = CursMsgWindow_input(&msgWindow);
  CursMsgWindow_close(&msgWindow);
  return key;
}

static int initIBsecServer()
{
  int     init = 0;
  int     g = 1;
  int     n = 64;
  int     rr = -1;

  CursInit();
  
  while(g)
  {
    init = inputInitItem();
    if (init == 0)
    {
      g = 0;
    }
    else if ((strcmp(masterKeyA1,masterKeyA2) != 0) ||(strcmp(masterKeyB1,masterKeyB2) != 0))
    {
      displayMsg("主密钥输入错误");
    }
    else
    {
      g = 0;
    }
  }

  CursEnd();

  return 0;
}

int main()
{
  initIBsecServer();
  printf("masterKeyA1 %s masterKeyA2 %s\n" , masterKeyA1 ,masterKeyA2);
  return 0;
}



