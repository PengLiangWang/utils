#ifndef _EPAY_TOKEN_H_
#define _EPAY_TOKEN_H_


struct _etoken_msg_t_
{
   long    mtype;
   int     rtype;
   int     ttype;
   int     tmax;
   int     tnum;
   int     tcode;
};

typedef struct _etoken_msg_t_  TTokenMsg;

#define ETOKEN_INCREASE_MAX      1
#define ETOKEN_DECREASE_MAX      2

#define ETOKEN_INCREASE_CURRENT   3
#define ETOKEN_DECREASE_CURRENT   4

#define ETOKEN_GET_STATUS         5

#define ETOKEN_ERR_SUCCESS        0
#define ETOKEN_ERR_FULL           1111
#define ETOKNE_ERR_DEFAULT        1112
#define ETOKNE_ERR_REQTYPE        1113


//ªÒ»°¡Ó≈∆
int TokenGet(int id, int *maxTrans, int *currTrans);

// Õ∑≈¡Ó≈∆
int TokenRecede(int id ,int *maxTrans, int *currTrans);

int TokenInfo(int id ,int *maxTrans, int *currTrans);

int TokenIncrease(int id ,int *maxTrans, int *currTrans);

int TokenDecrease(int id ,int *maxTrans, int *currTrans);


#endif
