#include <iconv.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "xlate.h"

#ifndef min
#define min(x,y) ((x) <= (y) ? (x) : (y))
#endif

struct TXlate
{
  TMemPool      *pool;
  char          *from;
  char          *to;
  char          *sbcs_table;
  iconv_t        ich;
};


static int XlateCleanup(void *convset)
{
  TXlate *old = (TXlate *)convset;

  if (old->ich != (iconv_t) - 1)
  {
    if (iconv_close(old->ich))
    {
      return errno ? errno : TTS_EINVAL;
    }
  }
  return TTS_SUCCESS;
}

static void check_sbcs(TXlate *convset)
{
  char      inbuf[256], outbuf[256];
  char     *inbufptr = inbuf;
  char     *outbufptr = outbuf;
  uint32    inbytes_left, outbytes_left;
  int       i;
  uint32    translated;

  for (i = 129; i < sizeof(inbuf); i++)
  {
    inbuf[i] = i;
  }

  inbytes_left = outbytes_left = sizeof(inbuf) / 2;
  
  translated = iconv(convset->ich, (char **)&inbufptr,&inbytes_left, &outbufptr, &outbytes_left);
  if (translated != (uint32)-1 && inbytes_left == 0 && outbytes_left == 0 )
  {
    convset->sbcs_table = MemPoolAllocEx(convset->pool, sizeof(outbuf));
    memcpy(convset->sbcs_table, outbuf, sizeof(outbuf));
    iconv_close(convset->ich);
    convset->ich = (iconv_t)-1;
  }
  else
  {
    iconv_close(convset->ich);
    convset->ich = iconv_open(convset->to, convset->from);
  }
}

static void make_identity_table(TXlate *convset)
{
  int i;

  convset->sbcs_table = MemPoolAllocEx(convset->pool, 256);
  for (i = 0; i < 256; i++)
  {
    convset->sbcs_table[i] = i;
  }
}

int XlateOpen(TXlate **convset,const char *to,const char *from,TMemPool *pool)
{
  int   rv;
  TXlate     *new;
  int        found = 0;

  *convset = NULL;

  new = (TXlate *)MemPoolAllocEx(pool, sizeof(TXlate));
  if (!new)
  {
    return TTS_ENOMEM;
  }

  new->pool = pool;
  new->to = (char *)m_strdup(pool, to);
  new->from = (char *)m_strdup(pool, from);
  if (!new->to || !new->from)
  {
    return TTS_ENOMEM;
  }

  if ((! found) && (strcmp(to, from) == 0))
  {
    /* to and from are the same */
    found = 1;
    make_identity_table(new);
  }

  if (!found)
  {
    new->ich = iconv_open(to, from);
    if (new->ich == (iconv_t)-1)
    {
      return errno ? errno : TTS_EINVAL;
    }
    found = 1;
    /*temporarilt not in user*/
    //check_sbcs(new);
  }
  else
  {
    new->ich = (iconv_t)-1;
  }

  if (found)
  {
    *convset = new;
    MemPoolRegisterCleanup(pool, (void *)new, XlateCleanup);
    rv = TTS_SUCCESS;
  }
  else
  {
    rv = TTS_EINVAL;
  }

  return rv;
}

int XlateSbGet(TXlate *convset, int *onoff)
{
  *onoff = convset->sbcs_table != NULL;
  return TTS_SUCCESS;
}

int XlateConvBuffer(TXlate *convset,const char *inbuf,uint32 *inbytes_left,char **outbuf,uint32 * outbytes_left)
{
  int  status = TTS_SUCCESS;
  uint32  outbytes = (*inbytes_left) * 2 + 1;
  char     *out = MemPoolAllocEx(convset->pool,outbytes);
  
  if ( outbytes_left != NULL )
  {
    *outbytes_left = outbytes;
  }

  if (convset->ich != (iconv_t)-1)
  {
    const char   *inbufptr = inbuf;
    char         *outbufptr = out;
    uint32      translated;

    translated = iconv(convset->ich, (char **)&inbufptr,inbytes_left, &outbufptr, &outbytes);
    if (translated == (uint32)-1)
    {
      int rv = errno;
      switch (rv)
      {
        //case E2BIG:   /* out of space on output */
        //  status = 0; /* change table lookup code below if you make this an error */
        //  break;

        case EINVAL:  /* input character not complete (yet) */
          status = TTS_EINVAL;
          break;

        case EILSEQ:  /* bad input byte */
          status = TTS_EINVAL;
          break;

        /* Sometimes, iconv is not good about setting errno. */
        case 0:
          status = TTS_EINVAL;
          break;

        default:
          status = rv;
          break;
      }
    }
  }
  else if (inbuf)
  {
    uint32 to_convert = *inbytes_left;//min(*inbytes_left, outbytes_left);
    uint32 converted = to_convert;
    char *table = convset->sbcs_table;

    while (to_convert)
    {
      *out = table[(unsigned char)*inbuf];
      ++out;
      ++inbuf;
      --to_convert;
    }
    *inbytes_left -= converted;
    //*outbytes_left -= converted;
  }

  if (status)
  {
    return status;
  }

  if ( outbytes_left != NULL )
  {
    *outbytes_left -= outbytes;
    *outbuf = (char *)m_strndup(convset->pool,(const char *)out,*outbytes_left);
  }
  else
  {
    outbytes = (*inbytes_left) * 2 + 1 - outbytes;
    *outbuf = (char *)m_strndup(convset->pool,(const char *)out,outbytes);
  }

  return status;
}

int XlateConvByte(TXlate *convset,unsigned char inchar)
{
  if (convset->sbcs_table)
  {
    return convset->sbcs_table[inchar];
  }
  else
  {
    return -1;
  }
}

int XlateClose(TXlate *convset)
{
  return MemPoolRunCleanup(convset->pool, convset, XlateCleanup);
}

