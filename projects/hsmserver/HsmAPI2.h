#ifndef _HSM_API2_H_
#define _HSM_API2_H_

#include "ttsys.h"
int Hsm2SetServer(char *serverInfo);


int Hsm2Close();

int Hsm2AddMainKey(char * index,char *keyData,char *checkVal);

int Hsm2LockMainKey(char * index);

int Hsm2GetMainKey(char *index,char *MainKeyA,char *CheckValA,char *MainKeyB,char *CheckValB,char *CheckVal);

int Hsm2GetRsaMainKey(char *index,char *RsaKey);

int Hsm2GetRsaPubKey(char *index,char *RsaKey);


int Hsm2GetWorkerKey(char *index,char *PinKey,char *CheckValP,char *MacKey,char *CheckValM);

int Hsm2GetUserPIN(char *indexA,char *userId,char *pin, char *indexB, char *newPin);

int Hsm2GetPINRsa(char *indexA,char *pan,char *pin, char *indexB, char *fA, char *fB, char *fC, char *acctNo, char *newPin);

int Hsm2GetPINRsa2(char *indexA,char *panData,char *pin, char *indexB, char *workKey, char *pinType, char *newPin);

int Hsm2GetPINRsa3(char *indexA,char *pin, char *newPin);

int Hsm2GetMac(char *indexA, char *workKey, char *data, int len, char *mac);

int Hsm2GetMacX99(char *indexA, char *workKey, char *data, int len, char *mac);


int Hsm2GetPsamKey(char *indexA, char *branchId, char *psamId, char *indexB, char *pkey, char *mac);

int Hsm2GetTrackData(char *masterKeyIdx, char *trkWorkKey, char *trackData, char *mac);

int Hsm2CalcPinRsa(char *rsaIndex, char *rsaPin, char *pinIndex, char *panData, char *fA, char *fB, char *fC, char *newPinData);

int Hsm56SetServer(char *serverInfo);


int Hsm56Close();

int Hsm56GetMac(char *indexA, char *workKey, char *data, int len, char *mac);

int Hsm56GetMasterKey(char *indexA, char *data, char *mac);

int Hsm56SetMasterKey(char *indexA, char *data, char *mac);

#endif



