#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "DesUtil.h"
#include "cursUtil.h"
#include "IBsecBox.h"
#include "IBsec.h"
#include "IBsecQueue.h"

static char keyPA[17],keyPB[17],keyC[17];

static CursInputItem  checkKeyMInputItem[] =
{
  {"��������Կ A : ",keyPA,4,2,17,0},
  {"��������Կ B : ",keyPB,6,2,17,0}
};

static CursInputWindow  checkKeyMInputWindow =
{
  "��������������ԿA����ԿB",
  9,50,6,16,
  2,checkKeyMInputItem
};

static char *nameList0[] =
{
  "���ӽڵ���Կ",
  "ɾ���ڵ���Կ",
  "------------",
  "���ڵ���Կ",
  "�����ڵ���Կ",
  "�ͷŽڵ���Կ"
};

static char *nameList1[] =
{
  "�޸���������Կ A",
  "�޸���������Կ B"
};

static char *nameList2[] =
{
  "������Կ�ļ�",
  "������Կ�ļ�"
};


static char *nameList3[] =
{
#ifdef _IBSM_DEBUG_
  "PIN ��Կ��֤",
  "PIN ��Կת����",
  "--------------",
#endif
  " �˳� "
};

static CursMenuBarItem  menuItemList[] =
{
  {" �ڵ���Կ���� ", 6, nameList0, 8, 16},
  {" ������Կ���� ", 2, nameList1, 4, 20},
  {" ��Կ�ļ����� ", 2, nameList2, 4, 16},
#ifdef _IBSM_DEBUG_
  {" ϵ    ͳ "    , 4, nameList3, 6, 18}
#else
  {" ϵ    ͳ "    , 1, nameList3, 3, 10}
#endif
};

static char YESORNO[2];
static char YesOrNoTitle[48];

static CursInputItem  yesOrNoInputItem[] =
{
  {YesOrNoTitle,YESORNO,4,2,2,1}
  //{"�Ƿ�ȷ��ɾ���ýڵ���Կ (Y/N) : ",YESORNO,4,2,2,1}
};

static CursInputWindow  yesOrNoInfoInputWindow =
{
  "ɾ���ڵ���Կ",
  7,50,10,17,
  1,yesOrNoInputItem
};

static CursMsgWindow  msgWindow =
{
  "��   Ϣ   ��", 16,66,4,10
};

static int displayMsg(char *msg)
{
  int  key;
  CursMsgWindow_open(&msgWindow);
  CursMsgWindow_printw(&msgWindow,"\n\n%s\n",msg);
  key = CursMsgWindow_input(&msgWindow);
  CursMsgWindow_close(&msgWindow);
  return key;
}

static int displayMsgV(char *msg,...)
{
  va_list   args;
  char      buf[897];
  int       key;

  va_start(args,msg);
  vsnprintf(buf,896,msg,args);
  va_end(args);

  CursMsgWindow_open(&msgWindow);
  CursMsgWindow_printw(&msgWindow,"\n\n%s\n",buf);
  key = CursMsgWindow_input(&msgWindow);
  CursMsgWindow_close(&msgWindow);
  return key;
}

static char *menu_title=" ��ȫ�ڵ���Կ������ ";

static char menu_title_buf[80];

static CursMenuBar  menu =
{
  menu_title_buf,
  24,80,0,0,
  4,menuItemList,
  19
};

static struct
{
  char  KeyA1[33];
  char  KeyA2[33];
  char  KeyB1[33];
  char  KeyB2[33];
  char  BASEKey11[33];
  char  BASEKey12[33];
  char  BASEKey21[33];
  char  BASEKey22[33];
  char  BASEKey[33];
  char  PINKey11[33];
  char  PINKey12[33];
  char  PINKey21[33];
  char  PINKey22[33];
  char  PINKey[33];
  char  FileName[32];
  char  CryMode[2];
}FKey;

static char   FKeyAA[17] = "AD13AkSG1uDAZD34";
static char   FKeyBB[17] = "78919229393lksdf";

static CursInputItem fKeyInputItem[] =
{
   {"��Կ�ļ�����  : ",FKey.FileName,3,2,32,1},
   {"������Կ A P1 : ",FKey.KeyA1,5,2,33,0},
   {"������Կ A P2 : ",FKey.KeyA2,6,2,33,0},
   {"������Կ B P1 : ",FKey.KeyB1,7,2,33,0},
   {"������Կ B P2 : ",FKey.KeyB2,8,2,33,0},
   {"BASE��Կ A P1 : ",FKey.BASEKey11,10,2,33,0},
   {"BASE��Կ A P2 : ",FKey.BASEKey12,11,2,33,0},
   {"BASE��Կ B P1 : ",FKey.BASEKey21,12,2,33,0},
   {"BASE��Կ B P2 : ",FKey.BASEKey22,13,2,33,0},
   {"PIK ��Կ A P1 : ",FKey.PINKey11,15,2,33,0},
   {"PIK ��Կ A P2 : ",FKey.PINKey12,16,2,33,0},
   {"PIK ��Կ B P1 : ",FKey.PINKey21,17,2,33,0},
   {"PIK ��Կ B P2 : ",FKey.PINKey22,18,2,33,0}
};

static CursInputWindow  creatKeyFileInputWindow =
{
  "��������Կ�ļ�",
  20,60,4,10,
  13,fKeyInputItem
};

static CursInputItem fInstallKeyInputItem[] =
{
   {"��Կ�ļ�����  : ",FKey.FileName,3,2,32,1},
   {"������Կ A P1 : ",FKey.KeyA1,5,2,33,0},
   {"������Կ A P2 : ",FKey.KeyA2,7,2,33,0},
   {"������Կ B P1 : ",FKey.KeyB1,9,2,33,0},
   {"������Կ B P2 : ",FKey.KeyB2,11,2,33,0}
};

static CursInputWindow  readKeyFileInputWindow =
{
  "��������Կ�ļ�",
  14,60,6,9,
  5,fInstallKeyInputItem
};

static int creatKeyCdata(char *node_id,char *key,char *data)
{
  memset(data,0,16);
  strncpy(data,node_id,10);
  endesmBlock(key,DES_MODE_2,data,16);
  return 0;
}

static int key_str_to_bin(char *key)
{
  char  data[17];
  int   i;

  for(i=0; key[i] != '\0'; ++i)
  {
    if ( !((key[i]>='0'&&key[i]<='9')||(key[i]>='A'&&key[i]<='F')||(key[i]>='a'&&key[i]<='f')) )
    {
      return -1;
    }
  }

  strToBin(data,key,16);
  memcpy(key,data,16);

  return 0;
}

static int key_str_to_key_data(char *key1,char *key2)
{
  int r;

  if (key1 != NULL)
  {
    r = key_str_to_bin(key1);
  }

  if (r != 0)
  {
    return r;
  }

  if (key2 != NULL)
  {
    r = key_str_to_bin(key2);
  }

  if (r)
  {
    return r;
  }

  return 0;
}

static int creat_key_file()
{
  int     add;
  int     r;
  int     g;
  FILE   *out;
  char    node_id[11];
  char    bin[25];
  char    str[49];
  char    crymode[2];
  long    ss[6];
  int     i;

  memset(&FKey,0,sizeof(FKey));

  for(i=0; i < 6; i++)
  {
    ss[i] = 0;
  }

  memset(node_id,0,11);
  memset(bin,0,25);
  memset(str,0,49);

  CursInputWindow_open(&creatKeyFileInputWindow);

  g = 1;

  while(g)
  {
    r=CursInputWindow_input(&creatKeyFileInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
        add = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        add = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&creatKeyFileInputWindow);

  if (add == 0)
  {
    return 0;
  }

  FKey.CryMode[0] = DES_MODE_2;

  if ( ( strcmp(FKey.KeyA1,FKey.KeyA2) != 0) || ( strcmp(FKey.KeyB1,FKey.KeyB2) != 0 ) )
  {
    displayMsg("�ļ�������Կ�������벻һ��");
    return -1;
  }

  out = fopen(FKey.FileName,"w");
  if (out == NULL)
  {
    displayMsg("����Կ�����ļ�����");
    return -1;
  }

  memset(node_id,0,11);
  r = IBsecGetSecNode(node_id,10);
  if (r != 0)
  {
    displayMsg("��ȡ���ذ�ȫ�ڵ�������");
    fclose(out);
    return -1;
  }

  if ( ( strcmp(FKey.PINKey11,FKey.PINKey12) != 0) || ( strcmp(FKey.PINKey21,FKey.PINKey22) != 0 ) )
  {
    displayMsg("PIK����Կ�������벻һ��");
    fclose(out);
    return -1;
  }

  if (strlen(FKey.PINKey11) != 32 || strlen(FKey.PINKey12) != 32)
  {
    displayMsg("PIK��Կ����û�дﵽ32λ,�������������Կ.");
    fclose(out);
    return -1;
  }

  if (strlen(FKey.PINKey21) != 32 || strlen(FKey.PINKey22) != 32)
  {
    displayMsg("PIK��Կ����û�дﵽ32λ,�������������Կ.");
    fclose(out);
    return -1;
  }


  key_str_to_key_data(FKey.KeyA1,FKey.KeyB1);

  endesmBlock(FKeyAA,FKey.CryMode[0],FKey.KeyA1,16);
  endesmBlock(FKeyBB,FKey.CryMode[0],FKey.KeyB1,16);

  fprintf(out,"%s ",node_id);
  endesm(FKey.KeyA1,FKey.CryMode[0],node_id);
  endesm(FKey.KeyB1,FKey.CryMode[0],node_id);

  for(i=0; i<4; ++i)
    node_id[i] ^= node_id[ 4 + i];

  endesm(FKey.KeyA1,FKey.CryMode[0],node_id);
  endesm(FKey.KeyB1,FKey.CryMode[0],node_id);

  binToStr(str,node_id,8);
  fprintf(out,"%s\n",str);

  time((time_t *)&ss[0]);
  ss[1] = (long)getpid();

  memcpy(bin,ss,24);

  dedesmBlock(FKeyAA,FKey.CryMode[0],bin,16);
  dedesmBlock(FKeyBB,FKey.CryMode[0],bin,16);

  key_str_to_key_data(FKey.BASEKey11,FKey.BASEKey21);

  for (i=0;i<16 ;i++ )
  {
    FKey.BASEKey[i] = FKey.BASEKey11[i] ^ FKey.BASEKey11[i];
  }

  endesmBlock((char *)ss,FKey.CryMode[0],FKey.BASEKey,16);

  fprintf(out,"WORKSBYS ");

  memcpy(bin,FKey.BASEKey,16);
  endesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  endesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  binToStr(str,bin,16);
  fprintf(out,"%s ",str);
  memcpy(bin,FKey.BASEKey,16);
  endesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  endesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  binToStr(str,bin,16);
  fprintf(out,"%s\n",str);


  key_str_to_key_data(FKey.PINKey11,FKey.PINKey21);

  for (i=0; i < 16 ;i++ )
  {
    FKey.PINKey[i] = FKey.PINKey11[i] ^ FKey.PINKey21[i];
  }

  endesmBlock((char *)ss,FKey.CryMode[0],FKey.PINKey,16);

  fprintf(out,"SYSPWORK ");

  memcpy(bin,FKey.PINKey,16);
  endesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  endesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  binToStr(str,bin,16);
  fprintf(out,"%s ",str);
  memcpy(bin,FKey.PINKey,16);
  endesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  endesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  binToStr(str,bin,16);
  fprintf(out,"%s\n",str);

  fprintf(out,"END\n");

  displayMsg("��������Կ�����ļ�����");

  fclose(out);

  return 0;
}

static int read_key_file()
{
  int    add;
  int    r;
  int    g;
  FILE  *out;
  char   node_id[11];
  char   node_id2[11];
  char   bin[17];
  char   str[97];
  char   str2[97];
  char   crymode[2];
  char   keyCdata[17];
  int    i;

  memset(node_id,0,11);

  memset(bin,0,17);
  memset(str,0,97);
  memset(str2,0,97);

  memset(&FKey,0,sizeof(FKey));
  memset(keyCdata,0,17);

  CursInputWindow_open(&readKeyFileInputWindow);
  g = 1;
  while(g)
  {
    r = CursInputWindow_input(&readKeyFileInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
        add = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        add = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&readKeyFileInputWindow);

  if (add == 0)
  {
    return 0;
  }

  if ((strcmp(FKey.KeyA1,FKey.KeyA2)!=0)||(strcmp(FKey.KeyB1,FKey.KeyB2)!=0))
  {
    displayMsg("�ļ�������Կ���벻һ��");
    return -1;
  }

  out = fopen(FKey.FileName,"r");
  if (out == NULL)
  {
    displayMsg("����Կ�����ļ�����");
    return -1;
  }

  FKey.CryMode[0] = DES_MODE_2;

  memset(node_id,0,11);
  r=IBsecGetSecNode(node_id,10);
  if (r != 0)
  {
    displayMsg("��ȡ���ؽڵ�������");
    fclose(out);
    return -1;
  }

  key_str_to_key_data(FKey.KeyA1,FKey.KeyB1);

  endesmBlock(FKeyAA,FKey.CryMode[0],FKey.KeyA1,16);
  endesmBlock(FKeyBB,FKey.CryMode[0],FKey.KeyB1,16);

  memset(node_id2,0,11);
  fscanf(out," %10s",node_id2);
  if (strcmp(node_id2,node_id) != 0)
  {
    goto E;
  }

  endesm(FKey.KeyA1,FKey.CryMode[0],node_id);
  endesm(FKey.KeyB1,FKey.CryMode[0],node_id);

  for(i=0; i < 4; ++i)
    node_id[i] ^= node_id[4 + i];

  endesm(FKey.KeyA1,FKey.CryMode[0],node_id);
  endesm(FKey.KeyB1,FKey.CryMode[0],node_id);

  binToStr(str,node_id,8);
  memset(str2,0,32);
  fscanf(out," %20s",str2);
  if (strcmp(str2,str)!=0)
  {
    goto E;
  }

  fscanf(out," %20s",str);
  if (strcmp(str,"WORKSBYS")!=0)
  {
    goto E;
  }

  fscanf(out," %60s",str);
  strToBin(bin,str,24);
  dedesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  dedesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  memcpy(FKey.BASEKey,bin,16);
  fscanf(out," %60s",str);
  strToBin(bin,str,16);
  dedesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  dedesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  if (memcmp(bin,FKey.BASEKey,16)!=0)
  {
    goto E;
  }

  fscanf(out," %20s",str);
  if (strcmp(str,"SYSPWORK")!=0)
  {
    goto E;
  }

  fscanf(out," %60s",str);
  strToBin(bin,str,24);
  dedesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  dedesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  memcpy(FKey.PINKey,bin,16);
  fscanf(out," %60s",str);
  strToBin(bin,str,16);
  dedesmBlock(FKey.KeyA1,FKey.CryMode[0],bin,16);
  dedesmBlock(FKey.KeyB1,FKey.CryMode[0],bin,16);
  if (memcmp(bin,FKey.PINKey,16)!=0)
  {
    goto E;
  }

  fscanf(out," %20s",str);
  if (strcmp(str,"END") != 0)
  {
    goto E;
  }

  fclose(out);

  creatKeyCdata("CARDPIN",keyC,keyCdata);
  r = IBsecAddKeyAllByC("CARDPIN","A","N",FKey.CryMode,FKey.BASEKey,FKey.BASEKey,keyCdata);
  if (r != 0)
  {
    displayMsg("��װ����Կ����");
    return -1;
  }

  r = IBsecAddKeyAllByC("CARDPIN","P","N",FKey.CryMode,FKey.PINKey,FKey.PINKey,keyCdata);
  if (r != 0)
  {
    displayMsg("��װ PIK ��Կ����");
    return -1;
  }

  displayMsg("��װ����Կ��PIK��Կ�ɹ�");
  return 0;

E:
  fclose(out);
  displayMsg("��ȡ����Կ�ļ�����");

  return 0;
}


static char oldPasswd[17];
static char newPasswdP1[17];
static char newPasswdP2[17];

static CursInputItem  changePitem[] =
{
  {"��������Կ    : ",oldPasswd,3,2,17,0},
  {"��������Կ P1 : ",newPasswdP1,5,2,17,0},
  {"��������Կ P2 : ",newPasswdP2,7,2,17,0}
};
static CursInputWindow  changePAwindow =
{
  "�޸���������Կ A",
  10,50,6,16,
  3,changePitem
};

static CursInputWindow  changePBwindow =
{
  "�޸���������Կ B",
  10,50,6,16,
  3,changePitem
};

static int clearPasswdBuf()
{
  memset(oldPasswd,0,17);
  memset(newPasswdP1,0,17);
  memset(newPasswdP2,0,17);
  changePitem[0].n = 0;
  changePitem[1].n = 0;
  changePitem[2].n = 0;
  return 0;
}

static int  changePA()
{
  int   r;
  int   g;
  int   change;

  clearPasswdBuf();
  CursInputWindow_open(&changePAwindow);
  g = 1;
  while(g)
  {
    r = CursInputWindow_input(&changePAwindow);
    switch(r)
    {
      case INPUTACTION_END:
        change = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        change = 0;
        g = 0;
        break;
    }
  }
  CursInputWindow_close(&changePAwindow);

  if (change)
  {
    if (strcmp(newPasswdP1,newPasswdP2)!=0)
    {
      displayMsg("����Կ�������");
    }
    else
    {
      char a[49],b[49];
      endesmBlock(MKEY,DES_MODE_2,oldPasswd,16);
      endesmBlock(MKEY,DES_MODE_2,newPasswdP1,16);

      if (IBsecChangePA(oldPasswd,newPasswdP1) != 0)
      {
        displayMsg("�޸���������Կ A ����");
      }
      else
      {
        memmove(keyC,newPasswdP1,16);
        dedesmBlock(MKEY,DES_MODE_2,keyC,16);
        displayMsg("�޸���������Կ A �ɹ�");
      }
    }
  }
  return 0;
}

static int changePB()
{
  int   r;
  int   g;
  int   change;

  clearPasswdBuf();

  CursInputWindow_open(&changePBwindow);
  g = 1;
  while(g)
  {
    r=CursInputWindow_input(&changePBwindow);
    switch(r)
    {
      case INPUTACTION_END:
        change = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        change = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&changePBwindow);
  if (change)
  {
    if (strcmp(newPasswdP1,newPasswdP2)!=0)
    {
      displayMsg("����Կ�������");
    }
    else
    {
      endesmBlock(MKEY,DES_MODE_2,oldPasswd,16);
      endesmBlock(MKEY,DES_MODE_2,newPasswdP1,16);
      if (IBsecChangePB(oldPasswd,newPasswdP1)!=0)
      {
        displayMsg("�޸���������Կ B ����");
      }
      else
      {
        memmove(keyC,newPasswdP1,16);
        dedesmBlock(MKEY,DES_MODE_2,keyC,16);
        displayMsg("�޸���������Կ B �ɹ�");
      }
    }
  }

  return 0;
}

static char nodeId[11];
static char keyType[2];
static char keyState[2];
static char cryMode[2];
static char keyA1[33];
static char keyA2[33];
static char keyB1[33];
static char keyB2[33];

static CursInputItem  keyInfoInputItem[] =
{
  {"�ڵ����    : ", nodeId,  3, 2, 11, 1},
  {"����Կ A P1 : ", keyA1,   5, 2, 33, 0},
  {"����Կ A P2 : ", keyA2,   7, 2, 33, 0},
  {"����Կ B P1 : ", keyB1,   9, 2, 33, 0},
  {"����Կ B P2 : ", keyB2,  11, 2, 33, 0}
};

static CursInputWindow  addKeyInputWindow=
{
  "���ӽڵ���Կ",
  14,54,6,18,
  5,keyInfoInputItem
};

static CursInputItem  defaultkeyInfoInputItem[] =
{
  {"�ڵ����  : ",nodeId, 4,2,11,1}
};

static CursInputWindow  batchDelKeyInputWindow =
{
  "ɾ���ڵ���Կ",
  7,50,6,18,
  1,defaultkeyInfoInputItem
};


static CursInputItem  checkKeyInputItem[] =
{
  {"�ڵ���� : ",nodeId,4,2,11,1},
  {"��Կ���� : ",keyType,6,2,2,1}
};

static CursInputWindow  checkKeyInputWindow =
{
  "���ڵ���Կ",
  9,50,6,18,
  2,checkKeyInputItem
};

static CursInputWindow  lockKeyInputWindow =
{
  "�����ڵ���Կ",
  9,50,6,18,
  2,checkKeyInputItem
};

static CursInputWindow  unlockKeyInputWindow =
{
  "�ͷŽڵ���Կ",
  9,50,6,18,
  2,checkKeyInputItem
};

static int clearKeyStateTypeCrymode()
{

  memset(keyState,0,2);
  memset(keyType,0,2);
  memset(cryMode,0,2);
}

static int clearKeyBuf()
{
  memset(keyA1,0,33);
  memset(keyA2,0,33);
  memset(keyB1,0,33);
  memset(keyB2,0,33);
  keyInfoInputItem[3].n = 0;
  keyInfoInputItem[4].n = 0;
  keyInfoInputItem[5].n = 0;
  keyInfoInputItem[6].n = 0;

  return 0;
}

#define ADD_KEY_A_RESULT  1
#define ADD_KEY_P_RESULT  2
#define ADD_KEY_M_RESULT  4
#define ADD_KEY_D_RESULT  8

static mypow(int x,int y)
{
  if(y == 0)
    return 1;
  if(y == 1)
    return x;
  else
    return mypow(x,y-1)*x;
}

static int logAddKeyResult(int *result,int i)
{
  *result |= mypow(2,i);
}

static void showAddKeyResult(int result)
{
  char msg[128];

  memset(msg,0,sizeof(msg));

  strcpy(msg,"���ӽڵ���Կ");
  if (result ==0 )
  {
    displayMsg("���ӽڵ���Կ�ɹ�.");
  }
  else
  {
    if(result&ADD_KEY_A_RESULT)
    {
      strcat(msg," A ");
    }
    if(result&ADD_KEY_P_RESULT)
    {
      strcat(msg," P ");
    }
    if(result&ADD_KEY_M_RESULT)
    {
      strcat(msg," M ");
    }
    if(result&ADD_KEY_D_RESULT)
    {
      strcat(msg," D ");
    }

    strcat(msg,"ʧ��");
    displayMsg(msg);
  }
}

static void showDelKeyResult(int result)
{
  char msg[128];

  memset(msg,0,sizeof(msg));

  strcpy(msg,"ɾ���ڵ���Կ");
  if (result ==0 )
  {
    displayMsg("ɾ���ڵ���Կ�ɹ�.");
  }
  else
  {
    if(result&ADD_KEY_A_RESULT)
    {
      strcat(msg," A ");
    }
    if(result&ADD_KEY_P_RESULT)
    {
      strcat(msg," P ");
    }
    if(result&ADD_KEY_M_RESULT)
    {
      strcat(msg," M ");
    }
    if(result&ADD_KEY_D_RESULT)
    {
      strcat(msg," D ");
    }

    strcat(msg,"ʧ��");
    displayMsg(msg);
  }
}

static int addNodeKey()
{
  int   rv;
  int   g,i;
  int   add = 0;
  char  keyCdata[17];
  int   addResult = 0;
  char  keyArray[4]={'A','P','M','D'};
  char  crymode[2];

  clearKeyBuf();
  memset(keyCdata,0,17);
  memset(nodeId,0,sizeof(nodeId));
  CursInputWindow_open(&addKeyInputWindow);

  g = 1;
  while(g)
  {
    rv = CursInputWindow_input(&addKeyInputWindow);
    switch(rv)
    {
      case INPUTACTION_END:
        add = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        add = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&addKeyInputWindow);
  if (add)
  {
    creatKeyCdata(nodeId,keyC,keyCdata);
    if (nodeId[0] == '\0')
      displayMsg("�ڵ���벻��Ϊ��");
    else if (strcmp(nodeId,"CARDPIN") == 0)
      displayMsg("����Ȩ���Ӹýڵ���Կ");
    else if (strcmp(keyA1,keyA2)!=0)
      displayMsg("��Կ A �������벻һ��,����");
    else if (strcmp(keyB1,keyB2)!=0)
      displayMsg("��Կ B �������벻һ��,����");
    else
    {
      if (strlen(keyA1) > 32)
      {
        displayMsg("��ԿA�������");
        return 0;
      }
      if (strlen(keyB1) > 32)
      {
        displayMsg("��ԿB�������");
        return 0;
      }

      key_str_to_key_data(keyA1,keyB1);

      keyType[0] = KEYTYPE_BASE;
      keyType[1] = 0;

      cryMode[0] = DES_MODE_2;
      cryMode[1] = 0;

      keyState[0] = IB_SEC_KEY_STAT0;
      keyState[1] = 0;

      if (IBsecAddKeyAllByC(nodeId,keyType,keyState,cryMode,keyA1,keyB1,keyCdata) != 0)
      {
        logAddKeyResult(&addResult,0);
      }

      for (i = 1;i < 4 ;i++ )
      {
        keyType[0]  = keyArray[i];
        keyType[1]  = 0;
        keyState[0] = 'N';
        keyState[1] = 0;
        memset(keyA1,0,sizeof(keyA1));
        memset(keyB1,0,sizeof(keyB1));
        crymode[0]  = cryMode[0];
        crymode[1]  = 0;

        if (IBsecAddKeyAllByC(nodeId,keyType,keyState,crymode,keyA1,keyB1,keyCdata) != 0)
        {
          logAddKeyResult(&addResult,i);
        }
      }
      showAddKeyResult(addResult);
    }
  }

  return 0;
}

static int delKeyYesOrNo(char *node_id)
{
  int  r;
  int  g;
  int  ok = 0;
  
  memset(YESORNO,0,sizeof(YESORNO));
  memset(YesOrNoTitle,0,sizeof(YesOrNoTitle));
  sprintf(YesOrNoTitle,"�Ƿ�ȷ��ɾ���ڵ� %s ��Կ (Y/N) : ",node_id);
  CursInputWindow_open(&yesOrNoInfoInputWindow);
  g = 1;
  while(g)
  {
    r=CursInputWindow_input(&yesOrNoInfoInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
        ok = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        ok = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&yesOrNoInfoInputWindow);
  if (ok == 1)
  {
    if (YESORNO[0] != 'Y' && YESORNO[0] != 'y')
    {
      ok = 0;
    }
  }

  return ok;
}
static int delNodeKey()
{
  int   r;
  int   g;
  int   del = 0;
  int   i;
  char  keyCdata[17];
  char  cryptmode[2];
  char  keyArray[4]={'A','P','M','D'};
  int   flag;
  int   delResult = 0;

  CursInputWindow_open(&batchDelKeyInputWindow);
  g = 1;
  while(g)
  {
    r = CursInputWindow_input(&batchDelKeyInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
         del = 1;
         g = 0;
         break;
      case INPUTACTION_CANCEL:
         del = 0;
         g = 0;
         break;
    }
  }

  CursInputWindow_close(&batchDelKeyInputWindow);
  if (del)
  {
    memset(keyCdata,0,17);
    creatKeyCdata(nodeId,keyC,keyCdata);
    if (nodeId[0]=='\0')
      displayMsg("�ڵ���벻��Ϊ��");
    /*else if (strcmp(nodeId,"CARDPIN") == 0)
      displayMsg("����Ȩɾ���ýڵ���Կ");*/
    else if (delKeyYesOrNo(nodeId) == 0)
      return 0;
    else
    {
      flag = 0;
      for(i = 0; i < 4; i++)
      {

        keyType[0] = keyArray[i];
        keyType[1] = 0;
        if (IBsecDelKeyByC(nodeId,keyType,keyCdata) != 0)
        {
          logAddKeyResult(&delResult,i);;
        }
      }
      showDelKeyResult(delResult);
    }
  }
  return 0;
}

static char *getKeyTypeStr(char *keyType)
{
  char  *str;

  if (keyType == NULL)
  {
    return NULL;
  }
  if (keyType[0] == 'A')
  {
    str = "����Կ";
    return str;
  }
  else if (keyType[0] == 'P')
  {
    str = "PIN��Կ";
    return str;
  }
  else if (keyType[0] == 'M')
  {
    str = "MAC��Կ";
    return str;
  }
  else if (keyType[0] == 'D')
  {
    str = "PKG��Կ";
    return str;
  }
  else
  {
    return NULL;
  }
}

static char *getCryModeStr(char *cryMode)
{
  char  *str;

  if (cryMode == NULL)
  {
    return NULL;
  }
  if (cryMode[0] == '0')
  {
    str = "����";
    return str;
  }
  else if (cryMode[0] == '1')
  {
    str = "DES";
    return str;
  }
  else if (cryMode[0] == '2')
  {
    str = "3DES";
    return str;
  }
  else if (cryMode[0] == '3')
  {
    str = "3DES";
    return str;
  }
  else
  {
    return NULL;
  }
}

static char *getKeyStateStr(char *keyState)
{
  char  *str;

  if (keyState == NULL)
  {
    return NULL;
  }
  if (keyState[0] == 'N')
  {
    str = "����";
    return str;
  }
  else if (keyState[0] == 'R')
  {
    str = "REQCHANGE";
    return str;
  }
  else if (keyState[0] == 'C')
  {
    str = "CHANGE_OK";
    return str;
  }
  else
  {
    return NULL;
  }
}

static int checkKey()
{
  int   r;
  int   g;
  int   check = 0;
  char  buf[256];
  int   flag;
  char  crymode[2];
  char  keyCdata[25];

  memset(keyCdata,0,25);
  memset(crymode,0,2);

  CursInputWindow_open(&checkKeyInputWindow);
  g = 1;
  while(g)
  {
    r = CursInputWindow_input(&checkKeyInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
        check = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        check = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&checkKeyInputWindow);
  if (check)
  {
    creatKeyCdata(nodeId,keyC,keyCdata);
    if (nodeId[0] == '\0')
      displayMsg("�ڵ���벻��Ϊ��");
    else if (keyType[0] == '\0')
      displayMsg("��Կ���Ͳ���Ϊ��");
    else if (IBsecGetKeyAllByC(nodeId,keyType,keyState,crymode,keyA1,keyB1,keyCdata) != 0)
      displayMsgV("�ڵ� %s ��Կ %s ������.",nodeId,keyType);
    else
    {
      sprintf(buf,"�ڵ���Կ��Ϣ: \n    �ڵ���� = %s\n    ��Կ���� = %s\n    �����㷨 = %s \n    ��Կ״̬ = %s\n",
          nodeId,getKeyTypeStr(keyType),getCryModeStr(crymode),getKeyStateStr(keyState));
      displayMsg(buf);
    }
  }
  return 0;
}

static int lockKey()
{
  int  r;
  int  g;
  int  lock = 0;

  CursInputWindow_open(&lockKeyInputWindow);
  g = 1;
  while(g)
  {
    r=CursInputWindow_input(&lockKeyInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
        lock = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        lock = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&lockKeyInputWindow);

  if (lock)
  {
    if (nodeId[0] == '\0')
      displayMsg("�ڵ���벻��Ϊ��");
    else if (strcmp(nodeId,"CARDPIN") == 0)
      displayMsg("����Ȩ�����ýڵ���Կ");
    else if (keyType[0] == '\0')
      displayMsg("��Կ���Ͳ���Ϊ��");
    else if (IBsecLockKey(nodeId,keyType) != 0)
      displayMsgV("�ڵ� %s ��Կ %s ������.",nodeId,keyType);
    else 
      displayMsgV("�����ڵ� %s ��Կ %s �ɹ�.",nodeId,keyType);
  }
  return 0;
}

static int unlockKey()
{
  int  r;
  int  g;
  int  unlock = 0;

  CursInputWindow_open(&unlockKeyInputWindow);
  g = 1;
  while(g)
  {
    r=CursInputWindow_input(&unlockKeyInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
        unlock = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        unlock = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&unlockKeyInputWindow);
  if (unlock)
  {
    if (nodeId[0] == '\0')
      displayMsg("�ڵ���벻��Ϊ��");
    else if (strcmp(nodeId,"CARDPIN") == 0)
      displayMsg("����Ȩ�ͷŸýڵ���Կ");
    else if (keyType[0] == '\0')
      displayMsg("��Կ���Ͳ���Ϊ��");
    else if (IBsecUnlockKey(nodeId,keyType)!=0)
      displayMsgV("�ڵ� %s ��Կ %s ������\n����Կû�б�����.",nodeId,keyType);
    else
      displayMsgV("�ͷŽڵ� %s ��Կ %s �ɹ�.",nodeId,keyType);
  }
  return 0;
}

static int checkKeyM()
{
  int  r;
  int  g;
  int  test;

  memset(keyPA,0,17);
  memset(keyPB,0,17);
  CursInputWindow_open(&checkKeyMInputWindow);
  g = 1;
  while(g)
  {
    r=CursInputWindow_input(&checkKeyMInputWindow);
    switch(r)
    {
      case INPUTACTION_END:
        test = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        test = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&checkKeyMInputWindow);
  if (test == 0)
    return -1;

  endesmBlock(MKEY,DES_MODE_2,keyPA,16);
  endesmBlock(MKEY,DES_MODE_2,keyPB,16);
  if (IBsecCheckPA(keyPA)==0)
  {
    memmove(keyC,keyPA,16);
  }
  else if (IBsecCheckPB(keyPB)==0)
  {
    memmove(keyC,keyPB,16);
  }
  else
  {
    displayMsg("��������ԿУ���");
    return -2;
  }

  dedesmBlock(MKEY,DES_MODE_2,keyC,16);
  return 0;
}

#ifdef _IBSM_DEBUG_

static char cardno[32];
static char cardpin[20];
static char cardpinchecktype[2];
static char oldNodeId[11];

static CursInputItem  checkPinKeyInputItem[] = 
{
  {"�ڵ����  : ",oldNodeId,3,2,11,1},
  {"����      : ",cardno,5,2,20,1},
  {"�ͻ�����  : ",cardpin,7,2,17,0},
  {"��������  : ",cardpinchecktype,9,2,2,1}
};

static CursInputWindow  checkPinKeyInputWindow = 
{
  "PIN ��Կ��֤",
  12,45,6,18,
  4,checkPinKeyInputItem
};

static CursInputItem  changePinKeyInputItem[] = 
{
  {"�ڵ����  : ",oldNodeId,3,2,11,1},
  {"����      : ",cardno,5,2,20,1},
  {"�ͻ�����  : ",cardpin,7,2,17,0},
  {"�Է��ڵ�  : ",nodeId,9,2,11,1}
};

static CursInputWindow  changePinKeyInputWindow = 
{
  "PIN��Կת����",
  12,45,6,18,
  5,changePinKeyInputItem
};

static int checkPinKey()
{
  int    r;
  int    g;
  int    check = 0;
  char   str[20];
  char   buf[128];

  memset(cardpin,0,sizeof(cardpin));
  CursInputWindow_open(&checkPinKeyInputWindow);
  g=1;
  while(g)
  {
    r=CursInputWindow_input(&checkPinKeyInputWindow);
    switch(r)
    {
      case INPUTACTION_END: 
        check = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        check = 0;
        g = 0;
        break;
    }
  }

  CursInputWindow_close(&checkPinKeyInputWindow);
  if (check)
  {
    if (oldNodeId[0] == '\0')
      displayMsg("�ڵ���벻��Ϊ��");
    else if (cardpinchecktype[0]<'0'||cardpinchecktype[0]>'1')
      displayMsg("������ʹ���");
    else
    {
      if (cardpinchecktype[0] == '0')
      {
        r = IBsecEncryptPin(oldNodeId,cardno,cardpin);
      }
      else if (cardpinchecktype[0] == '1')
      {
        memset(str,0,sizeof(str));
        strToBin(str,cardpin,8);
        memset(cardpin,0,sizeof(cardpin));
        memcpy(cardpin,str,8);
        r = IBsecDecryptPin(oldNodeId,cardno,cardpin);
      }
      else
      {
         displayMsg("�������ͷǷ�,0:����,1:����");
         return 0;
      }
      if (r != 0)
        displayMsg("У�����");
      else
      {
        memset(str,0,sizeof(str));
        binToStr(str,cardpin,8);
        sprintf(buf,"У������ : %s",str);
        displayMsg(buf);
      }
    }
  }
  return 0;
}

static int changePinKey()
{
  int    r;
  int    g;
  int    check = 0;
  char   str[20];
  char   buf[128];

  memset(cardpin,0,sizeof(cardpin));
  CursInputWindow_open(&changePinKeyInputWindow);
  g=1;
  while(g)
  {
    r=CursInputWindow_input(&changePinKeyInputWindow);
    switch(r)
    {
      case INPUTACTION_END: 
        check = 1;
        g = 0;
        break;
      case INPUTACTION_CANCEL:
        check = 0;
        g = 0;
        break;
    }
  }
  CursInputWindow_close(&changePinKeyInputWindow);
  if (check)
  {
    if (oldNodeId[0] == '\0')
      displayMsg("�ڵ���벻��Ϊ��");
    else if (nodeId[0] == '\0')
      displayMsg("�Է��ڵ���벻��Ϊ��");
    else
    {
      memset(str,0,sizeof(str));
      strToBin(str,cardpin,8);
      memset(cardpin,0,sizeof(cardpin));
      memcpy(cardpin,str,8);
      r = IBsecExchangePin(oldNodeId,nodeId,cardno,cardpin);
      if (r != 0)
        displayMsg("У�����");
      else
      {
        memset(str,0,sizeof(str));
        binToStr(str,cardpin,8);
        sprintf(buf,"У������ : %s",str);
        displayMsg(buf);
      }
    }
  }
  return 0;
}

#endif

static int mainMenu()
{
  int   r;
  int   g;
  char  node[11];

  memset(node,0,sizeof(node));
  r = IBsecGetSecNode(node,10);
  if (r!=0)
    return -1;


  sprintf(menu_title_buf,"��%s��- �ڵ�(%s)",menu_title,node);


  CursMenuBar_open(&menu);
  g = 1;
  while(g)
  {
     r = CursMenuBar_input(&menu);
     switch(r)
     {
       case MENUACTION_SELECT:
         switch(menu.currentItem*100 + menu.item[menu.currentItem].currentItem)
         {
            case 0: addNodeKey();
              break;
            case 1: delNodeKey();
              break;
            case 3: checkKey();
              break;
            case 4: lockKey();
              break;
            case 5: unlockKey();
              break;

            case 100: changePA();
              break;
            case 101: changePB();
              break;

            case 200: creat_key_file();
              break;
            case 201: read_key_file();
              break;

#ifdef _IBSM_DEBUG_
            case 300: checkPinKey();
              break;
            case 301: changePinKey();
              break;
            case 303:
              g = 0;
              break;
#else
            case 300:
              g = 0;
              break;
#endif
            
      }
      CursMenuBar_redraw(&menu);
      break;
    case MENUACTION_END:
      g = 0;
      break;
    }
  }
  CursMenuBar_close(&menu);

  return 0;
}

int main(int argc,char *argv[])
{
  int  rv;
  extern char *CurrentProgram;

  CurrentProgram="IBsm";

  desInit(0);
  IBsecBoxInit();

  rv = sec_queue_init(0);
  if (rv != 0)
  {
    fprintf(stderr,"��Կ��������г�ʼ��ʧ��.\n");
    return -1;
  }

  CursInit();
  if(checkKeyM() != 0)
  {
    fprintf(stderr,"����Կ����\n");
    goto E;
  }

  mainMenu();

E:
  CursEnd();

  return 0;
}
