#ifndef _SYS_IB_SEC_H_
#define _SYS_IB_SEC_H_


typedef struct
{
  char   crypt_type[2];
  char   node_id[11];
  char   new_node_id[11];
  char   pin_data[24];
  char   account_no[20];
}IBPININFO;


typedef struct
{
  char   node_id[11];
  char   key_type[2];
  char   key_state[2];
  char   cry_mode[2];
  char   keyA[17];
  char   keyB[17];
  int    flag;
  char   keyC[17];
}IBKEYINFO;

typedef struct
{
  long    mtype;
  int     cmd;
  int     ans;
  union
  {
    IBKEYINFO   key;
    IBPININFO   pin;
  }info;
}IBCMDINFO;


#define IBANS_OK                 0
#define IBANS_ERR               -1


#define KEYTYPE_BASE            'A'
#define KEYTYPE_PIN             'P'
#define KEYTYPE_MAC             'M'
#define KEYTYPE_PKG             'D'


#define IBCMD_KEY_ADD            1
#define IBCMD_KEY_DEL            2
#define IBCMD_KEY_GET            3
#define IBCMD_KEY_UPDATE         4
#define IBCMD_KEY_LOCK           5
#define IBCMD_KEY_UNLOCK         6
#define IBCMD_KEY_CHANGE_PA      7
#define IBCMD_KEY_CHANGE_PB      8
#define IBCMD_GET_SEC_NODE       9
#define IBCMD_KEY_CHECK_PA      10
#define IBCMD_KEY_CHECK_PB      11
#define IBCMD_CRYPT_PIN         12
#define IBCMD_KEY_GET_C         13
#define IBCMD_EXIT              14


//NORMAL
#define IB_SEC_KEY_STAT0        'N'
//REQCHANGE
#define IB_SEC_KEY_STAT1        'R'
//CHANGE_OK
#define IB_SEC_KEY_STAT2        'C'   


//加密
#define PIN_CRYPT_TYPE_ENCRYPT  '0'
//解密
#define PIN_CRYPT_TYPE_DECRYPT  '1'
//转加密
#define PIN_CRYPT_TYPE_HOSTDES  '2'

#define KEYFLAG_LOCK             1


#define PIN_FORMAT_HEX           1
#define PIN_FORMAT_BIN           2


int IBsecInit();

//获取本地安全节点编码
int IBsecGetSecNode(char *node_id,int maxlen);
//根据KeyC获取密钥所有的信息
int IBsecGetKeyAllByC(char *node_id,char *type,char *keyState,char *cryMode,char *keyA,char *keyB,char *keyC);
//根据KeyC获取密钥基本的信息
int IBsecGetKeyByC(char *node_id,char *type,char *keyState,char *keyA,char *keyB,char *keyC);
//获取密码所有的信息
int IBsecGetKeyAll(char *node_id,char *type,char *keyState,char *cryMode,char *keyA,char *keyB);
//获取密钥基本的信息
int IBsecGetKey(char *node_id,char *type,char *keyState,char *keyA,char *keyB);
//根据KeyC增加密钥
int IBsecAddKeyAllByC(char *node_id,char *type, char *keyState,char *cryMode,char *keyA,char *keyB,char *keyC);
//根据KeyC增加密钥,算法类型为 DES_2
int IBsecAddKeyByC(char *node_id,char *type,char *keyState,char *keyA,char *keyB,char *keyC);
//根据KeyC更新密钥
int IBsecSetKeyAllByC(char *node_id,char *type,char *keyState,char *cryMode,char *keyA,char *keyB,char *keyC);
//锁定KEY
int IBsecLockKey(char *node_id,char *type);
//解锁KEY
int IBsecUnlockKey(char *node_id,char *type);
//删除KEY
int IBsecDelKeyByC(char *node_id,char *type,char *keyC);
//修改启动密钥A
int IBsecChangePA(char *old,char *new);
//修改启动密钥B
int IBsecChangePB(char *old,char *new);
//校验启动密钥A
int IBsecCheckPA(char *passwdA);
//校验启动密钥B
int IBsecCheckPB(char *passwdB);
//加密PIN
int IBsecEncryptPin(char *node_id,char *card_no,char *pin_data);
//解密PIN
int IBsecDecryptPin(char *node_id,char *card_no,char *pin_data);
//转加密PIN(ASNI X.98)
int IBsecExchangePin(char *old_node_id,char *new_node_id,char *card_no,char *pin_data);
//获取MAC密钥
int IBsecGetMacKey(char *node_id,char *keyState,char *keyA,char *keyB);
//获取PKG密钥
int IBsecGetPkgKey(char *node_id,char *keyState,char *keyA,char *keyB);
//把PIN转换成主机格式 in_pin_format = PIN_FORMAT_HEX or PIN_FORMAT_BIN
int IBsecGetPinData(char *node_id,char *card_no,char *in_pin_data,int in_pin_format,char *out_pin_data);

int IBsecEndesBlock(char *node_id,char *block,int len);
int IBsecDedesBlock(char *node_id,char *block,int len);
int IBsecCheckMac(char *node_id,char *block,int len,char *mac);
int IBsecCreateMac(char *node_id,char *block,int len,char *mac);

int IBsecExchangeUserPin(char *mk,char *pk,char *pin,char *card_no,char *node_id,int flag);

#endif
