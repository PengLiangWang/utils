#ifndef _HSM_API_H_
#define _HSM_API_H_

//���ü��ܻ���ַ
int HsmSetServer(char *hsmHost,int hsmPort);

//���ü��ܻ���ַ1
int HsmSetServer2(char *hsmHost);

//�򿪼��ܻ�
int HsmOpen();

//�رռ��ܻ�
int HsmClose();

//����KEY
int HsmAddMainKey(char * index,char *MainKey,char *checkVal,char *hsmResp);

//��ȡMasterKey
int HsmGetMainKey(char *index,char *MainKeyA,char *CheckValA,char *MainKeyB,char *CheckValB,char *CheckVal,char *hsmResp);

int HsmGetRsaMainKey(char *index,char *RsaKey,char *hsmResp);

//��ȡ������Կ
int HsmGetWorkerKey(char *index,char *PinKey,char *PinCheckValue,char *Mackey,char *MacCheckValue,char *hsmResp);


//����PIN
int HsmEnPIN(char *indexA,char *PinKey,char *pinData,char *AcctNo,char *newPinData,char *hsmResp);

//ת����
int HsmCalcPIN(char *indexA,char *PinKey,char *indexB,char *workKey,char *pinData,char *AcctNo,char *newPinData,char *hsmResp);

//ת����3DES
int HsmCalcPIN3DES(char *indexA,char *PinKey,char *indexB,char *factorA,char *factorB,char *factorC,char *pinData,char *AcctNo,char *newPinData,char *hsmResp);

//������ַ����
int FormatHsmHostString(char *host,char *hsmAddr,int *hsmPort);

//�ŵ�����
int FormatAccountFromTrack2(char *track2,char *acct);

//ECB MAC
int HsmCalcMAC(char *index,char *MacKey,char *data,int dataLen,char *MAC,char *hsmResp);

//�����û�����
int HsmEnUserPIN(char *indexA,char *userId,char *pinData, char *indexB, char *newPinData,char *hsmResp);


//ת����CARD PIN (20120708)
int HsmCalcPinRsa(char *indexA,char *panData,char *pinData, char *indexB, char *newPinData,char *hsmResp);

int HsmCalcPinRsa2(char *indexA, char *pinData, char *fA, char *fB, char *fC, char *newPinData,char *hsmResp);

typedef struct 
{
  char         hsmAddr[16];
  int          hsmPort;

  char         keyIdxA[6];
  char         pinKeyA[33];
  char         macKeyA[33];

  char         keyIdxB[6];
  char         pinKeyB[33];
  char         macKeyB[33];
  
  char         keyIdxC[6];
  char         factorA[17];
  char         factorB[17];
  char         factorC[17];

  char         keyIdxD[6];
  char         factorD[17];
  char         factorE[17];
  char         factorF[17];

}THsmAPI;


#endif



