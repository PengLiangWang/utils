#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <netdb.h>
#include "logger.h"
#include "hlist.h"
#include "DesUtil.h"
#include "IBsecQueue.h"
#include "IBsecBox.h"
#include "IBsecKeyData.h"
#include "IBsecUtil.h"

#include "cursUtil.h"
#include "d_malloc.h"

static char     dbUserName[16];
static char     dbUserPasswd[10];
static char     local_node_id[11];
static char     local_node_check_mark[17];

static char     MKEYA[17] = "AD13AkSG1uDAZD34";
static char     MKEYB[17] = "78919229393lksdf";

static char     masterKeyA1[17];
static char     masterKeyB1[17];
static char     masterKeyA2[17];
static char     masterKeyB2[17];

static DesKeyS  A_KS,B_KS,C_KS;

static  int     serverP = 0;
static  int     serverL = 0;

static int checkPKEY(char *op_data,char *key)
{
  char  data[17];

  memset(data,0,17);

  strncpy(data,op_data,16);

  endesmBlock(masterKeyA1,DES_MODE_2,data,16);

  if (memcmp(data,key,16) == 0)
  {
    return 0;
  }

  memset(data,0,17);

  strncpy(data,op_data,16);

  endesmBlock(masterKeyB1,DES_MODE_2,data,16);

  if (memcmp(data,key,16) == 0)
  {
    return 0;
  }

  return IBANS_ERR;
}

struct dbIBKEYINFO
{
  char  node_id[11];
  char  key_type[2];
  char  key_state[2];
  char  cry_mode[2];
  char  keyA[33];
  char  keyB[33];
};

typedef struct dbIBKEYINFO DBkeyInfo;

typedef struct
{
  IBKEYINFO  k;
  int        n;
} IBKEYINFON;

static int MaxKeyNum = 0;

static int HASHLISTSIZE = 4497;

static HList  kList;

int KeyInfoCmp(void *a,void *b)
{
  IBKEYINFO  *ka=(IBKEYINFO *)a;
  IBKEYINFO  *kb=(IBKEYINFO *)b;

  if(ka == NULL & &kb !=NULL)
  {
    return -1;
  }

  if(ka != NULL & &kb ==NULL)
  {
    return 1;
  }

  return strcmp(ka->node_id,kb->node_id) || strcmp(ka->key_type,kb->key_type);
}

int KeyInfoDel(void *a)
{
  if(a != NULL)
  {
    d_free(a);
    a = NULL;
  }
  return 0;
}

int KeyInfoHashValue(void *p)
{
 char          index_str[20];
 unsigned int  hashValue;
 IBKEYINFO     *k = (IBKEYINFO *)p;

 memset(index_str,0,sizeof(index_str));
 strcpy(index_str,k->node_id);
 strcat(index_str,k->key_type);

 hashValue = CalcHashnr((unsigned char *)(index_str),strlen(index_str));

 return (int) (hashValue % HASHLISTSIZE);
}

static IBKEYINFON *k_find(IBKEYINFO *k)
{
  IBKEYINFON *kf = NULL;

  HList_find(&kList,k,(void **)&kf);

  return kf;
}

int IBKEYINFOToDBkeyInfo(IBKEYINFO *k,DBkeyInfo *dk, DesKeyS *a,DesKeyS *b)
{
  char  key[17];

  memset(dk,0,sizeof(DBkeyInfo));
  memset(key,0,17);
  
  strncpy(dk->node_id,k->node_id,10);
  strncpy(dk->key_type,k->key_type,1);
  strncpy(dk->key_state,k->key_state,1);
  strncpy(dk->cry_mode,k->cry_mode,1);
  memmove(key,k->keyA,16);

  {
    int   i;
    for(i=0;i<8;++i) 
    {
      key[i] ^= dk->node_id[i];
    }
  }

  endesmBlockKeyS(a,key,16);
  endesmBlockKeyS(b,key,16);
  binToStr(dk->keyA,key,16);
  memset(key,0,17);
  memmove(key,k->keyB,16);

  {
    int   i;
    for(i=0;i<8;++i) 
    {
      key[i] ^= dk->node_id[i];
    }
  }
  endesmBlockKeyS(a,key,16);
  endesmBlockKeyS(b,key,16);
  binToStr(dk->keyB,key,16);
  return 0;
}

static int k_find_not_a(IBKEYINFO *k,IBKEYINFO *ka)
{
  IBKEYINFON  *kf=NULL;
  DBkeyInfo    db;
  int          r;

  HList_find(&kList,k,(void **)&kf);
  if (kf != NULL)
  {
    dedesmBlock(ka->keyA,ka->cry_mode[0],kf->k.keyA,16);
    dedesmBlock(ka->keyA,ka->cry_mode[0],kf->k.keyB,16);
    r = DeleteSecKeyList(kf->n,kf->k.node_id,kf->k.key_type);
    if (r != 0)
    {
      return IBANS_ERR;
    }

    IBKEYINFOToDBkeyInfo(&(kf->k),&db,&A_KS,&B_KS);
    r = WriteSecKeyList(kf->n,db.node_id,db.key_type,db.key_state,db.cry_mode,db.keyA,db.keyB);
    if (r != 0)
    {
      return IBANS_ERR;
    }
  }

  return 0;
}

static int k_add(IBCMDINFO *kc)
{
  int           r,i;
  DBkeyInfo     db;
  IBKEYINFON   *kInfo;
  IBKEYINFON   *k;
  IBKEYINFO     ka;
  IBKEYINFON   *p_ka;
  char          keyArray[3]={'P','M','D'};

  if (checkPKEY(kc->info.key.node_id,kc->info.key.keyC) != 0)
  {
    return IBANS_ERR; 
  }

  kInfo = (IBKEYINFON *)d_malloc(sizeof(IBKEYINFON));
  if (kInfo == NULL)
  {
    return IBANS_ERR;
  }

  memmove(&kInfo->k,&kc->info.key,sizeof(IBKEYINFO));
  k = k_find(&kInfo->k);
  if (k != NULL)
  {
    d_free(kInfo);
    return IBANS_ERR;
  }

  if (kInfo->k.key_type[0] != KEYTYPE_BASE)
  {
    memset(&ka,0,sizeof(ka));
    strcpy(ka.node_id,kInfo->k.node_id);
    ka.key_type[0] = KEYTYPE_BASE;

    p_ka = k_find(&ka);

    if (p_ka != NULL)
    {
      dedesmBlock(p_ka->k.keyA,p_ka->k.cry_mode[0],kInfo->k.keyA,16);
      dedesmBlock(p_ka->k.keyA,p_ka->k.cry_mode[0],kInfo->k.keyB,16);
    }
    else 
    {
      d_free(kInfo);
      return IBANS_ERR;
    }
  }
  else if (kInfo->k.key_type[0] == KEYTYPE_BASE)
  {

    for(i = 0; i < 3; i++)
    {
      memset(&ka,0,sizeof(ka));
      strcpy(ka.node_id,kInfo->k.node_id);
      ka.key_type[0] = keyArray[i];
      k_find_not_a(&ka,&(kInfo->k));
    }
  }

  IBKEYINFOToDBkeyInfo(&kInfo->k,&db,&A_KS,&B_KS);

  kInfo->n = MaxKeyNum;

  r = WriteSecKeyList(kInfo->n,db.node_id,db.key_type,db.key_state,db.cry_mode,db.keyA,db.keyB);
  if (r != 0)
  {
    d_free(kInfo);
    return IBANS_ERR;
  }
  
  r = HList_add(&kList,kInfo);
  if (r != 0)
  {
    d_free(kInfo);
    return IBANS_ERR;
  }
  
  ++MaxKeyNum;

  return IBANS_OK;
}

static int k_del(IBCMDINFO *kc)
{
  int          r;
  IBKEYINFON   *k;

  if (checkPKEY(kc->info.key.node_id,kc->info.key.keyC) != 0)
  {
    return IBANS_ERR;
  }

  k = k_find((IBKEYINFO *)&kc->info.key);
  if (k==NULL)
  {
    return IBANS_ERR;
  }

  r=DeleteSecKeyList(k->n,k->k.node_id,k->k.key_type);
  if (r != 0)
  {
    return IBANS_ERR;
  }

  r = HList_del(&kList,k,NULL);
  if (r != 0)
  {
    return IBANS_ERR;
  }
  return IBANS_OK;
}

static int k_update(IBCMDINFO *kc)
{
  int          r;
  DBkeyInfo    db;
  IBKEYINFON  *k;
  IBKEYINFON   kInfo;
  IBKEYINFO    ka;
  IBKEYINFON  *p_ka;
  int          flag;
  char         checkKey[17];

  k = k_find(&kc->info.key);
  if (k == NULL)
  {
    return IBANS_ERR;
  }

  memmove(&kInfo.k,&kc->info.key,sizeof(IBKEYINFO));
  kInfo.n = k->n;

  memcpy(checkKey,kInfo.k.keyC,16);
  if (kInfo.k.key_type[0] != KEYTYPE_BASE)
  {
    memset(&ka,0,sizeof(ka));
    strcpy(ka.node_id,kInfo.k.node_id);
    ka.key_type[0] = KEYTYPE_BASE;

    p_ka = k_find(&ka);
    if (p_ka != NULL)
    {
      dedesmBlock(p_ka->k.keyA,p_ka->k.cry_mode[0],kInfo.k.keyA,16);
      dedesmBlock(p_ka->k.keyA,p_ka->k.cry_mode[0],kInfo.k.keyB,16);
      dedesmBlock(p_ka->k.keyA,p_ka->k.cry_mode[0],checkKey,16);
    }

  }

  if ((memcmp(k->k.keyA,checkKey,16) != 0 ) && (memcmp(k->k.keyB,checkKey,16) != 0 ) &&
      (checkPKEY(kc->info.key.node_id, kc->info.key.keyC) != 0))
  {
    memset(kc->info.key.keyA,0,16);
    memset(kc->info.key.keyB,0,16);
    return IBANS_ERR;
  }

  IBKEYINFOToDBkeyInfo(&kInfo.k,&db,&A_KS,&B_KS);

  r = WriteSecKeyList(kInfo.n,db.node_id,db.key_type,db.key_state,db.cry_mode,db.keyA,db.keyB);
  if (r != 0)
  {
    return IBANS_ERR;
  }

  strcpy(k->k.key_state,kInfo.k.key_state);
  strcpy(k->k.cry_mode,kInfo.k.cry_mode);
  memmove(k->k.keyA,kInfo.k.keyA,16);
  memmove(k->k.keyB,kInfo.k.keyB,16);
  
  return IBANS_OK;
}

static int k_get(IBCMDINFO *kc)
{
  int          r;
  IBKEYINFON  *k;

  k = k_find(&kc->info.key);
  if (k == NULL)
  {
    return IBANS_ERR;
  }

  if ((kc->info.key.key_type[0] == KEYTYPE_PIN) || (kc->info.key.key_type[0] == KEYTYPE_BASE))
  {
    return IBANS_ERR;
  }

  memcpy(kc->info.key.keyA,k->k.keyA,16);
  memcpy(kc->info.key.keyB,k->k.keyB,16);
  strncpy(kc->info.key.key_state,k->k.key_state,1);
  strncpy(kc->info.key.cry_mode,k->k.cry_mode,1);

  return IBANS_OK;
}

static int k_get_c(IBCMDINFO *kc)
{
  int          r;
  IBKEYINFON  *k;
  IBKEYINFO    ka;
  IBKEYINFON  *p_ka;

  k = k_find(&kc->info.key);
  if (k==NULL)
  {
    return IBANS_ERR;
  }

  if (kc->info.key.key_type[0] == KEYTYPE_BASE)
  {
    memcpy(kc->info.key.keyA,"INVALID",16);
    memcpy(kc->info.key.keyB,"INVALID",16);
    return IBANS_OK;
  }

  memcpy(kc->info.key.keyA,k->k.keyA,16);
  memcpy(kc->info.key.keyB,k->k.keyB,16);
  strncpy(kc->info.key.key_state,k->k.key_state,1);
  strncpy(kc->info.key.cry_mode,k->k.cry_mode,1);
  kc->info.key.flag = k->k.flag;

  memset(&ka,0,sizeof(ka));
  strcpy(ka.node_id,kc->info.key.node_id);
  ka.key_type[0] = KEYTYPE_BASE;

  p_ka = k_find(&ka);
  if (p_ka != NULL)
  {
    endesmBlock(p_ka->k.keyA,p_ka->k.cry_mode[0],kc->info.key.keyA,16);
    endesmBlock(p_ka->k.keyA,p_ka->k.cry_mode[0],kc->info.key.keyB,16);
  }
 
  if ((memcmp(kc->info.key.keyA,kc->info.key.keyC,16) != 0) &&
      (memcmp(kc->info.key.keyB,kc->info.key.keyC,16) != 0) &&
      (checkPKEY(kc->info.key.node_id, kc->info.key.keyC) != 0) )
  {
    memset(kc->info.key.keyA,0,16);
    memset(kc->info.key.keyB,0,16);
    return IBANS_ERR;
  }

  return IBANS_OK;
} 

static int k_lock(IBCMDINFO *kc)
{
  int         r;
  IBKEYINFON  *k;

  k = k_find(&kc->info.key);
  if (k==NULL)
  {
    return IBANS_ERR;
  }

  memmove(&kc->info.key,&k->k,sizeof(IBKEYINFO));
  if (k->k.flag & KEYFLAG_LOCK)
  {
    return IBANS_ERR;
  }
  k->k.flag |= KEYFLAG_LOCK;
  return IBANS_OK ;
}

static int k_unlock(IBCMDINFO *kc)
{
  int          r;
  IBKEYINFON  *k;

  k = k_find(&kc->info.key);
  if (k==NULL)
  {
    return IBANS_ERR;
  }
  k->k.flag &= ~KEYFLAG_LOCK;
  return IBANS_OK ;
}

static int change_PA_f(void *p)
{
  IBKEYINFON  *k;
  DBkeyInfo   db;
  
  k = (IBKEYINFON *)p;
  IBKEYINFOToDBkeyInfo(&k->k,&db,&C_KS,&B_KS);
  k->n = MaxKeyNum++;
  
  return WriteSecKeyList(k->n,db.node_id,db.key_type,db.key_state,db.cry_mode,db.keyA,db.keyB);

}

static int creatMark(char *node_id,DesKeyS *aKs,DesKeyS *bKs,char *mark)
{
  char  s[16];

  memset(s,0,16);
  strncpy(s,node_id,10);
  endesmBlockKeyS(aKs,s,16);
  endesmBlockKeyS(bKs,s,16);
  binToStr(mark,&s[8],8);
  
  return 0;
}

static int save_master_key()
{
  char  l_dbUserName[16];
  char  l_dbUserPasswd[10];
  char  l_masterKeyA1[17];
  char  l_masterKeyB1[17];
  int   r;
  
  r = IBsecBoxLoadMKey(l_dbUserName,l_dbUserPasswd,l_masterKeyA1,l_masterKeyB1);
  if( r == 0 )
  {
    IBsecBoxSaveMKey(dbUserName,dbUserPasswd,masterKeyA1,masterKeyB1);
  }

  return 0;
}

static int k_changePA(IBCMDINFO *kc)
{
  int   r;
  char  mark[17];
  char  key[17];

  memmove(key,kc->info.key.keyA,16);
  
  /*
  {
    char a[33],b[33];
    binToStr(a,key,16);
    dedesmBlock(MKEY,DES_MODE_2,key,16);

    endesmBlock(MKEYA,DES_MODE_2,key,16);
  
    binToStr(a,key,16);
    desmSetKey(key,DES_MODE_2,&C_KS);

    if (memcmp(&C_KS,&A_KS,sizeof(C_KS))!=0)
    {
      return IBANS_ERR;
    }

  }
  */
  dedesmBlock(MKEY,DES_MODE_2,key,16);
  endesmBlock(MKEYA,DES_MODE_2,key,16);
  desmSetKey(key,DES_MODE_2,&C_KS);
  if (memcmp(&C_KS,&A_KS,sizeof(C_KS))!=0)
  {
    return IBANS_ERR;
  }
  
  memmove(key,kc->info.key.keyB,16);
  dedesmBlock(MKEY,DES_MODE_2,key,16);
  endesmBlock(MKEYA,DES_MODE_2,key,16);

  desmSetKey(key,DES_MODE_2,&C_KS);
  creatMark(local_node_id,&C_KS,&B_KS,mark);
  r = OpenSecKeyFileAndClean();
  if (r!=0)
  {
    return IBANS_ERR;
  }
  r = WriteSecKeyM(local_node_id,mark);
  if (r!=0)
  {
    goto E;
  }

  MaxKeyNum = 0;

  r = HList_step(&kList,change_PA_f);
  if (r!=0)
  {
    goto E;
  }

  CloseSecKeyFile();

  memmove(&A_KS,&C_KS,sizeof(C_KS));
  
  dedesmBlock(MKEYA,DES_MODE_2,key,16);
  
  memmove(masterKeyA1,key,16);
  
  save_master_key();
  
  return IBANS_OK;

E:
  CloseSecKeyFile();
  
  return IBANS_ERR;
}

static int change_PB_f(void *p)
{
  IBKEYINFON  *k;
  DBkeyInfo    db;

  k = (IBKEYINFON *)p;
  IBKEYINFOToDBkeyInfo(&k->k,&db,&A_KS,&C_KS);
  k->n = MaxKeyNum++;
  return WriteSecKeyList(k->n,db.node_id,db.key_type,db.key_state,db.cry_mode,db.keyA,db.keyB);
}

static int k_changePB(IBCMDINFO *kc)
{
  int   r;
  char  node_id[16];
  char  mark[17];
  char  key[17];

  memmove(key,kc->info.key.keyA,16);
  dedesmBlock(MKEY,DES_MODE_2,key,16);
  endesmBlock(MKEYB,DES_MODE_2,key,16);
  desmSetKey(key,DES_MODE_2,&C_KS);
  
  if (memcmp(&C_KS,&B_KS,sizeof(C_KS)) != 0)
  {
    return IBANS_ERR;
  }

  memmove(key,kc->info.key.keyB,16);
  dedesmBlock(MKEY,DES_MODE_2,key,16);
  endesmBlock(MKEYB,DES_MODE_2,key,16);
  desmSetKey(key,DES_MODE_2,&C_KS);

  creatMark(local_node_id,&A_KS,&C_KS,mark);
  
  r = OpenSecKeyFileAndClean();
  if (r != 0)
  {
    return IBANS_ERR;
  }
  
  r = WriteSecKeyM(local_node_id,mark);
  if (r != 0)
  {
    goto E;
  }

  MaxKeyNum = 0;
  
  r = HList_step(&kList,change_PB_f);
  if (r != 0)
  {
    goto E;
  }

  CloseSecKeyFile();
  
  memmove(&B_KS,&C_KS,sizeof(C_KS));
  
  dedesmBlock(MKEYB,DES_MODE_2,key,16);
  
  memmove(masterKeyB1,key,16);
  
  save_master_key();
  
  return IBANS_OK ;

E:
  CloseSecKeyFile();
  
  return IBANS_ERR;
}

static int k_get_sec_node(IBCMDINFO *kc)
{
  
  strncpy(kc->info.key.node_id,local_node_id,10);
  return IBANS_OK;
}


static int k_checkPA(IBCMDINFO *kc)
{
  int   r;
  char  key[16];

  memset(key,0,sizeof(key));
  memmove(key,kc->info.key.keyA,16);
  dedesmBlock(MKEY,DES_MODE_2,key,16);
  endesmBlock(MKEYA,DES_MODE_2,key,16);
  desmSetKey(key,DES_MODE_2,&C_KS);

  if (memcmp(&C_KS,&A_KS,sizeof(C_KS))!=0)
  {
    return IBANS_ERR;
  }

  return IBANS_OK;
}

static int k_checkPB(IBCMDINFO *kc)
{
  int   r;
  char  key[16];


  memmove(key,kc->info.key.keyB,16);
  dedesmBlock(MKEY,DES_MODE_2,key,16);
  endesmBlock(MKEYB,DES_MODE_2,key,16);
  desmSetKey(key,DES_MODE_2,&C_KS);
  if (memcmp(&C_KS,&B_KS,sizeof(C_KS))!=0)
  {
    return IBANS_ERR;
  }

  return IBANS_OK;
}

static int ServerExit = 0;

static int s_exit(IBCMDINFO *kc)
{
  ServerExit = 1;
  
  return IBANS_OK ;
}

static int get_pin_key(char *node_id,char *key_state,char *cry_mode,char *keyA,char *keyB)
{
  IBKEYINFO   key;
  IBKEYINFON *k;

  memset(&key,0,sizeof(key));
  strcpy(key.node_id,node_id);
  key.key_type[0] = KEYTYPE_PIN;

  k = k_find(&key);
  if (k == NULL)
  {
    return -1;
  }

  *key_state = k->k.key_state[0];
  *cry_mode= k->k.cry_mode[0];
  

  memmove(keyA,k->k.keyA,16);
  memmove(keyB,k->k.keyB,16);
  return 0;
}

static int k_crypt_pin(IBCMDINFO *kc)
{
  int  rv;

  rv = PinAcctCrypt(kc->info.pin.crypt_type[0],kc->info.pin.account_no,
                    kc->info.pin.pin_data,kc->info.pin.node_id,
                    kc->info.pin.new_node_id,get_pin_key);

  return rv;
}

static struct CmdList_t
{
  int cmdNo;
  int (*cmdF)(IBCMDINFO *kc);
} 
cmdList[] =
{
  {IBCMD_GET_SEC_NODE,    k_get_sec_node},
  {IBCMD_KEY_GET,         k_get},
  {IBCMD_KEY_GET_C,       k_get_c},
  {IBCMD_KEY_ADD,         k_add},
  {IBCMD_KEY_DEL,         k_del},
  {IBCMD_KEY_UPDATE,      k_update},
  {IBCMD_KEY_LOCK,        k_lock},
  {IBCMD_KEY_UNLOCK,      k_unlock},
  {IBCMD_KEY_CHANGE_PA,   k_changePA},
  {IBCMD_KEY_CHANGE_PB,   k_changePB},
  {IBCMD_KEY_CHECK_PA,    k_checkPA},
  {IBCMD_KEY_CHECK_PB,    k_checkPB},
  {IBCMD_CRYPT_PIN,       k_crypt_pin},
  {IBCMD_EXIT,            s_exit},
  {-1,                    NULL}
};

static int execCmd(IBCMDINFO *kc)
{
  int i;

  i = 0;
  
  while(cmdList[i].cmdF != NULL)
  {
    if (cmdList[i].cmdNo == kc->cmd)
    {
      kc->ans = (*cmdList[i].cmdF)(kc);
      return kc->ans ;
    }
    else 
    {
      ++i;
    }
  }

  kc->ans = IBANS_ERR;
  return kc->ans;
}

static  int cmdLoop()
{
  IBCMDINFO kc;

  memset(&kc,0,sizeof(kc));
  
  while(sec_queue_recv_cmd(&kc) == 0)
  {
    execCmd(&kc);

    if (sec_queue_send_ans(&kc)!=0)
    {
      return -1;
    }

    if (ServerExit)
    {
      return 0;
    }
    memset(&kc,0,sizeof(kc));
  }

  return -1;
}

static int checkMark(char *node_id,char *mark)
{
  char  cmark[17];

  creatMark(node_id,&A_KS,&B_KS,cmark);
  
  if (strcmp(mark,cmark) != 0)
  {
    return -1;
  }

  return 0;
}

static int getDBstr(char *s,char *src,int maxSize)
{
  int i;

  memset(s,0,maxSize);
  strncpy(s,src,maxSize - 1);

  for(i = strlen(s) - 1; i >= 0; --i)
  {
    if (s[i]==' ')
    {
      s[i]='\0';
    }
  }

  return (strlen(s));
}

static int loadKeyM(char *node_id,char *mark)
{
  getDBstr(local_node_id,node_id,11);
  strncpy(local_node_check_mark,mark,16);

  return 0;
}

static int loadKey(int n,char *node_id,char *key_type,char *key_state,char *cry_mode,char *keyA,char *keyB)
{ 
  int          r;
  IBKEYINFON  *kInfo;

  kInfo = (IBKEYINFON *)d_malloc(sizeof(IBKEYINFON));
  if (kInfo == NULL)
  {
    return -1;
  }

  memset(kInfo,0,sizeof(IBKEYINFON));
  kInfo->n = n;
  if (kInfo->n >= MaxKeyNum)
  {
    MaxKeyNum = kInfo->n + 1;
  }

  getDBstr(kInfo->k.node_id,node_id,11);

  if(key_state[0] == IB_SEC_KEY_STAT2)
  { 
    key_state[0] = IB_SEC_KEY_STAT0;
    r = WriteSecKeyList(n,kInfo->k.node_id,key_type,key_state,cry_mode,keyA,keyB);
    if ( r != 0)
    {
      return -1;
    }
  }

  strncpy(kInfo->k.key_type,key_type,1);
  strncpy(kInfo->k.key_state,key_state,1);
  strncpy(kInfo->k.cry_mode,cry_mode,1);
  strToBin(kInfo->k.keyA,keyA,16);
  
  dedesmBlockKeyS(&B_KS,kInfo->k.keyA,16);
  dedesmBlockKeyS(&A_KS,kInfo->k.keyA,16);

  {
    int i; 
    for(i=0;i<8;++i)
    {
      kInfo->k.keyA[i]^=kInfo->k.node_id[i];
    }
  }

  strToBin(kInfo->k.keyB,keyB,16);
  dedesmBlockKeyS(&B_KS,kInfo->k.keyB,16);
  dedesmBlockKeyS(&A_KS,kInfo->k.keyB,16);

  {
    int i; 
    for(i=0;i<8;++i)
    {
      kInfo->k.keyB[i]^=kInfo->k.node_id[i];
    }
  }
  
  r = HList_add(&kList,kInfo);
  if (r != 0)
  {
    d_free(kInfo);
    return -1;
  }

  return 0;
}

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
  char  *ib_node_name;

  ib_node_name = getenv("IBSECNODE");
  if (ib_node_name != NULL)
  {
    strncpy(localNODE,ib_node_name,10);
  }

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

static int setMasterKey(char *keyA,char *keyB)
{
  endesmBlock(MKEYA,DES_MODE_2,keyA,16);
  endesmBlock(MKEYB,DES_MODE_2,keyB,16);
  desmSetKey(keyA,DES_MODE_2,&A_KS);
  desmSetKey(keyB,DES_MODE_2,&B_KS);

  dedesmBlock(MKEYA,DES_MODE_2,keyA,16);
  dedesmBlock(MKEYB,DES_MODE_2,keyB,16);
  
  return 0;
}

static int initIBsecDB()
{
  int   r;
  char  mark_str[17];

  setMasterKey(masterKeyA1,masterKeyB1);
  creatMark(localNODE,&A_KS,&B_KS,mark_str);
  
  r = OpenSecKeyFileAndClean();
  if (r != 0)
  {
    goto E;
  }

  r = WriteSecKeyM(localNODE,mark_str);
  if (r != 0)
  {
    goto E;
  }

  CloseSecKeyFile();

  r = IBsecBoxClean();
  if (r != 0)
  {
    goto E;
  }

  return 0;

E:
  CursInit();
  displayMsg("密钥数据库出错");
  CursEnd();
  
  return 1;
}

static int initIBsecServer()
{
  int     init = 0;
  int     g = 1;
  int     n = 64;
  int     rr = -1;

  CursInit();
  desInit(0);
  
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
  
  if (init)
  {
    rr = initIBsecDB();
    if (rr != 0)
    {
      int rv;
      rv = IBsecBoxSaveMKey(dbUserName,dbUserPasswd,masterKeyA1,masterKeyB1);
      if (rv != 0)
      {
        return rv;
      }
    }
  }
  
  return rr;
}

static CursInputItem  IBsecLoadInputItem[]
  ={
    {"主启动密钥A     : ",masterKeyA1,  4,2,17,0},
    {"主启动密钥B     : ",masterKeyB1,  6,2,17,0}
  };

static CursInputWindow  IBsecLoadInputWindow
  ={
    "启动密钥系统",
    9,50,5,10,
    2,IBsecLoadInputItem
  };

static int IBsecLoadInput()
{
  int   r;
  int   g;
  int   load;

  CursInputWindow_open(&IBsecLoadInputWindow);
  g = 1;
  while(g)
  {
    r = CursInputWindow_input(&IBsecLoadInputWindow);
    switch(r)
    {
      case INPUTACTION_END:   
        g = 0;
        load = 1; 
        break;
      case INPUTACTION_CANCEL:
        g = 0;
        load = 0;
        break;
    }
  }

  CursInputWindow_close(&IBsecLoadInputWindow);
  
  return load;
}

static int IBsecLoadDB()
{
  int rv;

  rv = LoadSecKeyFile(loadKeyM,loadKey);
  if (rv != 0)
  {
    printf("\n");
    printf("  系统错误 : 装载密钥表出错.\n");
    printf("  原    因 : (1) 密钥表不存在.\n");
    printf("             (2) 密钥表非法.\n");
    printf("             (3) 系统还没有初始化.\n");
    return 1;
  }

  rv = checkMark(local_node_id,local_node_check_mark);
  if (rv != 0)
  {
    printf("\n");
    printf("  系统错误:主启动密钥非法.\n");
    printf("  原    因 : (1) 主启动密钥不正确.\n");
    printf("             (2) 主启动密钥不存在.\n");
    return 1;
  }

  return 0;
}


void Term_signal(int sid)
{
  HList_clean(&kList,&KeyInfoDel);
  sec_queue_remove();
  exit(0);
}

static int startSecServer(int input)
{
  int    r;
  int    load;
  pid_t  pid;
  int    lineNo;
  int    rr,i;
  int    n = 64;

  extern char *CurrentProgram;

  CurrentProgram = "IBsec";

  r = sec_queue_init(1);
  if (r != 0)
  {
    printf("\n");
    printf("  系统错误:创建安全队列失败.\n");
    printf("  原    因:(1) IBsec进程可能已经启动.\n");
    printf("           (2) IBsec的控制文件不存在.\n");
    return 2;
  }

  desInit(0);

  if (input)
  {
    r = -1;
  }
  else 
  {
    r = IBsecBoxLoadMKey(dbUserName,dbUserPasswd,masterKeyA1,masterKeyB1);
  }
  if (r != 0)
  {
    CursInit();

    load = IBsecLoadInput();
    
    CursEnd();
    
    if (!load)
    {
      return 1;
    }
  }

  setMasterKey(masterKeyA1,masterKeyB1);
  
  if(input)
  {
    r = IBsecBoxSaveMKey(dbUserName,dbUserPasswd,masterKeyA1,masterKeyB1);
    if(r)
    {
      return 1;
    }
  }
    
  pid = fork();
  if (pid < 0)
  {
    return 2;
  }

  if (pid != 0)
  {
    IBCMDINFO kc;

    memset(&kc,0,sizeof(kc));
    kc.mtype = (long)getpid();
    
    r = sec_queue_recv_ans(&kc);
    if (r < 0)
    {
      return 2;
    }

    if (kc.ans != IBANS_OK)
    {
      return 3;
    }

    return 0;
  }

  setsid();

  signal ( SIGTERM , Term_signal);
  signal ( SIGINT  , SIG_IGN );
  signal ( SIGPIPE , SIG_IGN );
  signal ( SIGQUIT , SIG_IGN );
  signal ( SIGHUP  , SIG_IGN );
  signal ( SIGCLD  , SIG_IGN );

  r = sec_queue_lock();
  if (r != 0)
  {
    printf("\n");
    printf("  系统错误:不能锁定安全队列.\n");
    printf("  原    因:(1) IBsec进程可能已经启动.\n");
    printf("           (2) IBsec的控制文件不存在.\n");

    goto E;
  }

  serverP = 1;

  HList_new(&kList,KeyInfoHashValue,KeyInfoCmp,HASHLISTSIZE);

  serverL = 1;

  r = IBsecLoadDB();

E:
  {
    IBCMDINFO kc;
    int       rr;

    memset(&kc,0,sizeof(kc));
    kc.mtype = (long)getppid();
    
    if (r == 0)
    {
      kc.ans = IBANS_OK;
    }
    else
    {
      kc.ans = IBANS_ERR;
    }
    
    rr = sec_queue_send_ans(&kc);
    if (rr < 0)
    {
      printf("\n");
      printf("  系统错误:安全队列初始化失败.\n");
      printf("  原    因:安全队列不存在或者系统出错,请检查日志.\n");
      return 2;
    }
  }

  if (r != 0)
  {
    return 2;
  }


  cmdLoop();
  
  return 0;
}

int main(int argc,char *argv[])
{
  int   r = 0;
  
  memset(masterKeyA1,0,16);
  memset(masterKeyB1,0,16);

  if((getenv("IBSECCTRLFILE")) == NULL)
  {
    printf("系统错误: 环境变量 IBSECCTRLFILE 没有设置.\n");
    return -1;
  }

  if((getenv("IBSECKEYDB")) == NULL)
  {
    printf("系统错误: 环境变量 IBSECKEYDB 没有设置.\n");
    return -1;
  }

  if((getenv("IBSECNODE")) == NULL)
  {
    printf("系统错误: 环境变量 IBSECNODE 没有设置.\n");
    return -1;
  }

  if ((argc==2) && (strcmp(argv[1],"-INIT") == 0))
  {
    r = initIBsecServer();
  }
  else if ((argc==2)&&(strcmp(argv[1],"-i")==0))
  {
    r = startSecServer(1);
  }
  else if ((argc==2)&&(strcmp(argv[1],"-SQ")==0))
  {
    printf("\n暂不提供该功能.\n");
  }
  else
  {
    r = startSecServer(0);
  }

  if (serverP)
  {
    sec_queue_remove();
  }

  if (serverL)
  {
    HList_clean(&kList,&KeyInfoDel);
  }

  return r;
}

