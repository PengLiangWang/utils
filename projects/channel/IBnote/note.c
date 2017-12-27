#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include "logger.h"
#include "note.h"
#include "note_queue.h"
#include "note_error.h"

/*
 * 向索引或值的字符串写入一块字符串
 * int note_putstr(int *n,char *buf,const char *str)
 * 输入参数:
 *          int   *n;   buf当前的位置
 *          char *buf;  索引或值的 BUFFER
 *          char *str;  原始域
 *
 */

int note_putstr(int *n,char *buf,const char *str)
{
  if( *n >= NOTEBUFSIZE - 1 )
  {
    return -1;
  }

  if( *n != 0 )
  {
    buf[(*n)++] = '&';
  }

  while ( *n < NOTEBUFSIZE - 2 )
  {
    switch(*str)
    {
      case '\0':
        return 0;
      case '&':
      case '\\':
        buf[(*n)++] = '\\';
      default:
        buf[(*n)++] = *(str++);
    }
  }

  return 0;
}

#define STAT_INDEX   0
#define STAT_GET     1

static int   stat = -1;
static int   n;
static char  index_str[NOTEBUFSIZE];
static char  value_str[NOTEBUFSIZE];

void note_clean()
{

  stat = STAT_INDEX;
  n = 0;
  memset(index_str,0,NOTEBUFSIZE);
  memset(value_str,0,NOTEBUFSIZE);
}

int note_index(const char *value)
{
  int rv;

  if(stat != STAT_INDEX)
  {
    note_clean();
  }

  rv = note_putstr(&n,index_str,value);
  if(rv)
  {
    logger(__FILE__,__LINE__,"note_index() => note_putstr() error %d",rv);
    return rv;
  }

  return 0;
}

int note_v_index(const char *value1,...)
{
  va_list    ap;
  char     *str;
  int        rv;

  note_clean();

  rv = note_index(value1);
  if(rv)
  {
    return rv;
  }

  va_start(ap,value1);
  while((str = va_arg(ap,char*)) != NULL)
  {
    rv = note_index(str);
    if(rv)
    {
      va_end(ap);
      return rv;
    }
  }

  va_end(ap);
  return 0;
}

int note_get(int size,char *value)
{
  int  rv;
  int  m;

  if(stat != STAT_GET)
  {
    rv = note_queue_get(index_str,value_str);
    if(rv)
    {
      return rv;
    }
    n = 0;
    stat = STAT_GET;
  }

  if(value_str[n] == '\0')
  {
    value[0] = '\0';
    return NOTE_NOTFOUND;
  }

  m = 0;
  
  while(n < NOTEBUFSIZE)
  {
    switch(value_str[n])
    {
      case '&':
        n++;
        goto E;
      case '\0':
        goto E;
      case '\\':
        if (n >= NOTEBUFSIZE)
        {
          goto E;
        }
      default:
        if (m >= size - 1)
        {
          value[m] = '\0';
          return NOTE_DATASIZEERROR;
        }
        value[m++]=value_str[n++];
        break;
    }
  }

E:
  value[m] = 0;
  return 0;
}

int note_v_get(int size1,char *value1,...)
{
  va_list    ap;
  char     *str;
  int      size;
  int        rv;
  
  rv = note_get(size1,value1);
  if(rv)
  {
    return rv;
  }
 
  va_start(ap,value1);
  while ( (size = va_arg(ap,int)) != 0)
  {
    str = va_arg(ap,char *);
    rv = note_get(size,str);
    if(rv)
    {
      va_end(ap);
      return rv;
    }
  }
  va_end(ap);
  return 0;
}

int note_v_put(const char *value1,...)
{
  va_list  ap;
  char     *str;
  int      rv;
  int      old_n;

  old_n = n;

  n = 0;
  
  memset(value_str,0,NOTEBUFSIZE);

  rv = note_putstr(&n,value_str,value1);
  if ( rv != 0 )
  {
    goto E;
  }

  va_start(ap,value1);
  
  while( ( str = va_arg(ap,char *) ) != NULL )
  {
    rv = note_putstr(&n,value_str,str);
    if (rv != 0)
    {
      va_end(ap);
      goto E;
    }
  }

  va_end(ap);
  rv = note_queue_put(index_str,value_str);
E:
  n = old_n;

  return rv;
}

