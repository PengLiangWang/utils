/*
 This Include File "opt_data.h" 
 Genenated By
 Application <comAction> v1.0 
 with the com file "opt_data.def".
 Create: Sat Sep  8 02:42:29 2007
*/

#ifndef _IB_opt_data_H_
#define _IB_opt_data_H_

#include "RCI.h"

typedef struct opt_rcv_data
{
  int       number;
  char      name[11];
  int       age;
  char      address[24];
} OPT_RCV_DATA;

int IB_RCI_PKG_OPT_RCV_DATA(void *v_data, char *buf, int buflen, int *useP, int in_out);

typedef struct opt_snd_data
{
  int       retcode;
  double    amount;
} OPT_SND_DATA;

int IB_RCI_PKG_OPT_SND_DATA(void *v_data, char *buf, int buflen, int *useP, int in_out);

#endif
