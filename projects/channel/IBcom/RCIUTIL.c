#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "RCI.h"
#include "RCIUTIL.h"
#include "IBcom.h"

int rci_in_out_str(char *str,int size,char *buf,int buflen,
            int *use,int in_out)
{
  int  n;

  n = *use;

  if ( n < 0 )
  {
    return -900;
  }

  switch(in_out)
  {
    case PACKET_IN:
      if (n > 0)
      {
        if (n < buflen-1)
        {
          buf[n++] = '&';
        }
        else 
        {
          logger(__FILE__,__LINE__,"buflen=%d n=%d",buflen,n);
          return -1;
        }
      }
      while( (*str != '\0') && (size > 0) )
      {
        if (*str == '&')
        {
          if (n < buflen - 3)
          {
            buf[n++] = '%';
            buf[n++] = '2';
            buf[n++] = '6';
          }
          else 
          {
            return -1;
          }
        }
        else if (*str == '%')
        {
          if (n < buflen - 3)
          {
            buf[n++] = '%';
            buf[n++] = '2';
            buf[n++] = '5';
          }
          else 
          {
            return -1;
          }
        }
        else if (n < buflen - 1)
        {
          buf[n++] = *str;
        }
        else
        {
          return -1;
        }
        ++str;
        --size;
      }
      break;
    case PACKET_OUT:
      if (n > 0)
      {
        if ((n > buflen - 1) || (buf[n] != '&'))
        {
          logger(__FILE__,__LINE__,"buflen=%d n=%d,buf=[%s]",buflen,n,buf);
          return -10;
        }
        else 
        {
          ++n;
        }
      }

      while((size > 0) && (n < buflen) && (buf[n] != '&'))
      {
        if (buf[n] == '%')
        {
          if ( n > buflen - 3)
          {
            return -11;
          }
          else
          {
            n++;
            strToBin(str,&buf[n],1);
            n += 2;
          }
        }
        else
          *str = buf[n++];

        --size;
        ++str;
      }

      *str = '\0';
      break;

    default:
      return -13;
  }

  *use = n;

  return 0;
}

int rci_in_out_bin(char *str,int size,char *buf,int buflen,
            int *use,int in_out)
{
  int  n;

  n = *use;

  if (n < 0)
  {
    return -1;
  }

  switch(in_out)
  {
    case PACKET_IN:
      if ( n > 0 )
      {
        if (n<buflen-1)
        {
          buf[n++] = '&';
        }
        else
        {
          return -1;
        }
      }

      while(size > 0)
      {
        if (*str == '&')
        {
          if ( n < buflen - 3)
          {
            buf[n++] = '%';
            buf[n++] = '2';
            buf[n++] = '6';
          }
          else 
          {
            return -1;
          }
        }
        else if (*str == '%')
        {
          if (n < buflen - 3)
          {
            buf[n++] = '%';
            buf[n++] = '2';
            buf[n++] = '5';
          }
          else
          {
            return -1;
          }
        }
        else if (n < buflen - 1)
        {
          buf[n++] = *str;
        }
        else 
        {
          return -1;
        }
        ++str;
        --size;
      }
      break;
    case PACKET_OUT:
      if (n > 0 )
      {
        if ((n > buflen - 1) || (buf[n] != '&'))
        {
          return -1;
        }
        else
        {
          ++n;
        }
      }

      while((size > 0) && (n < buflen) && (buf[n] != '&'))
      {
        if (buf[n] == '%')
        {
          if (n > buflen-3)
          {
            return -1;
          }
          else
          {
            n++;
            strToBin(str,&buf[n],1);
            n += 2;
          }
        }
        else
        {
          *str = buf[n++];
        }

        --size;
        ++str;
      }
      break;
    default:
      return -1;
  }

  *use = n;
  return 0;
}

int rci_in_out_short(short *value,int size,char *buf,int buflen,
            int *use,int in_out)
{
  int   r;
  char  s[32];

  switch(in_out)
  {
    case PACKET_IN:
      sprintf(s,"%d",(int)*value);
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_IN);
      break;
    case PACKET_OUT:
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_OUT);
      if ( r== 0)
      {
        *value = (short)atoi(s);
      }
      break;
    default:
      return -1;
  }
  return r;
}

int rci_in_out_int(int *value,int size,char *buf,int buflen,
            int *use,int in_out)
{
  int   r;
  char  s[32];

  switch(in_out)
  {
    case PACKET_IN:
      sprintf(s,"%d",*value);
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_IN);
      break;
    case PACKET_OUT:
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_OUT);
      if (r == 0)
      {
        *value = atoi(s);
      }
      break;
    default:
      return -1;
  }
  return r;
}

int rci_in_out_char(char *value,int size,char *buf,int buflen,
            int *use,int in_out)
{
  int   r;
  char  s[32];

  switch(in_out)
  {
    case PACKET_IN:
      sprintf(s,"%c",*value);
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_IN);
      break;
    case PACKET_OUT:
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_OUT);
      if (r == 0)
      {
        *value = s[0];
      }
      break;
    default:
      return -1;
  }
  return r;
}

int rci_in_out_long(long *value,int size,char *buf,int buflen,
            int *use,int in_out)
{
  int   r;
  char  s[32];

  switch(in_out)
  {
    case PACKET_IN:
      sprintf(s,"%ld",*value);
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_IN);
      break;
    case PACKET_OUT:
      r = rci_in_out_str(s,31,buf,buflen,use,PACKET_OUT);
      if (r == 0)
      {
        *value = atol(s);
      }
      break;
    default:
      return -1;
  }
  return r;
}

static void kill_zero(char *s)
{
  char   *p;
  int    n;

  p = strchr(s,'.');
  if (p == NULL)
  {
    return ;
  }

  for (n = (strlen(s) - 1); n > 0; --n)
  {
    if (s[n] == '.')
    {
      s[n] = '\0'; 
      return;
    }
    else if (s[n] == '0')
    {
      s[n] = '\0';
    }
    else 
      return ;
  }

  return ;
}

int rci_in_out_double(double *value,int size,char *buf,int buflen,
            int *use,int in_out)
{
  int   r;
  char  s[128];

  switch(in_out)
  {
    case PACKET_IN:
      sprintf(s,"%.9lf",*value);
      kill_zero(s);
      r = rci_in_out_str(s,63,buf,buflen,use,PACKET_IN);
      break;
    case PACKET_OUT:
      r = rci_in_out_str(s,63,buf,buflen,use,PACKET_OUT);
      if (r == 0)
      {
        *value = atof(s);
      }
      break;
    default:
      return -1;
  }
  return r;
}

int IB_RCI_PKG_ENV(void *v_data, char *buf, int buflen, int *useP, int in_out)
{
  int     r;
  RCI_ENV  *data;

  data = (RCI_ENV *)v_data;

  r = rci_in_out_str(data->message_type, 4, buf, buflen, useP, in_out);
  if (r != 0)
    goto E;

  r = rci_in_out_str(data->trans_code, 6, buf, buflen, useP, in_out);
  if (r != 0)
    goto E;

  r = rci_in_out_str(data->local_node_id, 16, buf, buflen, useP, in_out);
  if (r != 0)
    goto E;

  r = rci_in_out_str(data->remote_node_id, 16, buf, buflen, useP, in_out);
  if (r != 0)
    goto E;
E:
  return r;

}
