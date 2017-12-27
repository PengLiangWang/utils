#ifndef _HSM_API_H_
#define _HSM_API_H_

int HsmSetServer(char *hsmHost,int hsmPort);

int HsmSetServer2(char *hsmHost);

int HsmOpen();

int HsmClose();

int HsmAddMainKey(char * index,char *MainKey,char *checkVal,char *hsmResp);

int HsmGetMainKey(char *index,char *MainKeyA,char *CheckValA,char *MainKeyB,char *CheckValB,char *CheckVal,char *hsmResp);

int HsmGetRsaMainKey(char *index,char *RsaKey,char *hsmResp);

int HsmGetWorkerKey(char *index,char *PinKey,char *PinCheckValue,char *Mackey,char *MacCheckValue,char *hsmResp);


int HsmEnPIN(char *indexA,char *PinKey,char *pinData,char *AcctNo,char *newPinData,char *hsmResp);

int HsmCalcPIN(char *indexA,char *PinKey,char *indexB,char *workKey,char *pinData,char *AcctNo,char *newPinData,char *hsmResp);

int HsmCalcPIN3DES(char *indexA,char *PinKey,char *indexB,char *factorA,char *factorB,char *factorC,char *pinData,char *AcctNo,char *newPinData,char *hsmResp);

int FormatHsmHostString(char *host,char *hsmAddr,int *hsmPort);
int FormatAccountFromTrack2(char *track2,char *acct);

int HsmCalcMAC(char *index,char *MacKey,char *data,int dataLen,char *MAC,char *hsmResp);

int HsmEnUserPIN(char *indexA,char *userId,char *pinData, char *indexB, char *newPinData,char *hsmResp);


int HsmCalcPinRsa(char *indexA,char *panData,char *pinData, char *indexB, char *fA, char *fB, char *fC, char *newPinData,char *hsmResp);

int HsmCalcPinRsa2(char *indexA,char *panData,char *pinData, char *workKey, char *pinType, char *indexB, char *newPinData,char *hsmResp);

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



