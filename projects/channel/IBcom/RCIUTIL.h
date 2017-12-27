#ifndef _IB_RCI_UTIL_H_
#define _IB_RCI_UTIL_H_

int rci_in_out_str(char *str,int size,char *buf,int buflen,
            int *use,int in_out);
int rci_in_out_short(short *value,int size,char *buf,int buflen,
            int *use,int in_out);
int rci_in_out_char(char *value,int size,char *buf,int buflen,
            int *use,int in_out);
int rci_in_out_int(int *value,int size,char *buf,int buflen,
            int *use,int in_out);
int rci_in_out_long(long *value,int size,char *buf,int buflen,
            int *use,int in_out);
int rci_in_out_double(double *value,int size,char *buf,int buflen,
            int *use,int in_out);


#endif
