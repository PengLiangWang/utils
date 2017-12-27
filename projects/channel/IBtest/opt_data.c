/* 
 This C File "opt_data.c" 
 Genenated By
 Application <comAction> v1.0
 with the com file "opt_data.def".
 Create: Sat Sep  8 02:42:29 2007
*/

#include <stdio.h>
#include "opt_data.h"

int IB_RCI_PKG_OPT_RCV_DATA(void *v_data, char *buf, int buflen, int *useP, int in_out)
{
  int    r;
  OPT_RCV_DATA *data;
  data=(OPT_RCV_DATA *)v_data;

  r = rci_in_out_int(&data->number, sizeof(int), buf, buflen, useP, in_out);
  if (r != 0)
      goto E;

  r = rci_in_out_str(data->name, 10, buf, buflen, useP, in_out);
  if (r != 0)
      goto E;

  r = rci_in_out_int(&data->age, sizeof(int), buf, buflen, useP, in_out);
  if (r != 0)
      goto E;

  r = rci_in_out_str(data->address, 23, buf, buflen, useP, in_out);
  if (r != 0)
      goto E;
E:
  return(r);
}

int IB_RCI_PKG_OPT_SND_DATA(void *v_data, char *buf, int buflen, int *useP, int in_out)
{
  int    r;
  OPT_SND_DATA *data;
  data=(OPT_SND_DATA *)v_data;

  r = rci_in_out_int(&data->retcode, sizeof(int), buf, buflen, useP, in_out);
  if (r != 0)
      goto E;

  r = rci_in_out_double(&data->amount, sizeof(double), buf, buflen, useP, in_out);
  if (r != 0)
      goto E;
E:
  return(r);
}
