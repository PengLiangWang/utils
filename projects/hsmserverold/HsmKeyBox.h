#ifndef _HSM_KEY_BOX_H_
#define _HSM_KEY_BOX_H_


#pragma pack (1)
struct hsm_key_item
{
  int    keyIdx;
  char   keyType;
  char   keyFlag;
  int    keyLen;
  char   keyDataA[257];
  char   keyDataB[257];
  char   keyStat;
};
#pragma pack ()


typedef struct hsm_key_item  THsmKeyItem;

typedef struct hsm_key_item  THsmKeyInfo;

#define HSM_KEY_MAX      256

struct hsm_key_vector
{
  char         HsmStat;
  int          KeyCount;
  THsmKeyItem  item[HSM_KEY_MAX];
};

typedef struct hsm_key_vector  THsmKeyVector;

//int HsmKeyBoxSetDebug(int debug);

int HsmKeyBoxSetDatabase(char *dbName,char *dbUser,char *dbPasswd);

int HsmKeyBoxSetVector(THsmKeyVector *HsmKeyVector);

int HsmKeyBoxLoadKey();

int HsmKeyBoxUpdateKey(THsmKeyInfo  *hsmKeyInfo);

int HsmKeyBoxGetKey(THsmKeyInfo  *hsmKeyInfo);



#endif





