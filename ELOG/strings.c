#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "strings.h"

int isspaceline(char *line)
{
  while(*line!='\0')
  {
    switch(*line)
    {
      case '\r':
      case '\n':
      case '#':
        return 1;

      case ' ':
      case '\t':
        ++line;
        break;
      default:
        return 0;
    }
  }

  return 1;
}


char *m_strdup(TMemPool *a, const char *s)
{
  char  *res;
  int    len;

  if (s == NULL)
  {
    return NULL;
  }
  len = strlen(s) + 1;
  res = MemPoolAllocEx(a, len);
  memcpy(res, s, len);
  return res;
}

char * m_strndup(TMemPool *a, const char *s, int n)
{
  char *res;

  if (s == NULL)
  {
    return NULL;
  }
  res = MemPoolAllocEx(a, n + 1);
  memcpy(res, s, n);
  res[n] = '\0';
  return res;
}

char * m_strcat( TMemPool *a,...)
{
  char *cp, *argp, *res;
  int   len = 0;

  va_list adummy;

  va_start(adummy, a);

  while ((cp = va_arg(adummy, char *)) != NULL)
  {
    len += strlen(cp);
  }

  va_end(adummy);

  res = (char *) MemPoolAllocEx(a, len + 1);
  cp = res;
  *cp = '\0';

  va_start(adummy, a);

  while ((argp = va_arg(adummy, char *)) != NULL)
  {
    strcpy(cp, argp);
    cp += strlen(argp);
  }

  va_end(adummy);

  return res;
}


char * m_itoa(TMemPool *p, int n)
{
  const int BUFFER_SIZE = sizeof(int) * 3 + 2;
  char *buf = MemPoolAllocEx(p, BUFFER_SIZE);
  char *start = buf + BUFFER_SIZE - 1;
  int negative;
  if (n < 0)
  {
    negative = 1;
    n = -n;
  }
  else
  {
    negative = 0;
  }

  *start = 0;
  do
  {
    *--start = '0' + (n % 10);
    n /= 10;
  } while (n);

  if (negative)
  {
    *--start = '-';
  }

  return start;
}

char * m_ltoa(TMemPool *p, long n)
{
  const int BUFFER_SIZE = sizeof(long) * 3 + 2;
  char *buf = MemPoolAllocEx(p, BUFFER_SIZE);
  char *start = buf + BUFFER_SIZE - 1;
  int negative;
  if (n < 0)
  {
    negative = 1;
    n = -n;
  }
  else
  {
    negative = 0;
  }
  *start = 0;
  do
  {
    *--start = (char)('0' + (n % 10));
    n /= 10;
  } while (n);

  if (negative)
  {
    *--start = '-';
  }

  return start;
}

int m_strcmp(char *a,char *b)
{
  if(a == NULL || b == NULL)
  {
    return -1;
  }

  while(*a == *b && *a != '\0' && *b != '\0')
  {
    a++;
    b++;
  }

  if(*a == *b)
  {
    return 0;
  }

  return -1;
}

char * m_cpystrn(char *dst, const char *src, uint32 dst_size)
{

  char *d, *end;

  if (dst_size == 0) 
  {
    return dst;
  }

  d = dst;
  end = dst + dst_size - 1;

  for (; d < end; ++d, ++src) 
  {
    if (!(*d = *src))
    {
      return d;
    }
  }

  *d = '\0';

  return d;
}

char * m_trim(char* szDest)
{
 int    n, nLen;
 char   *ptr;

 if ( szDest != NULL )
 {
   ptr = szDest;

   while ( *ptr )
   {
     if( *ptr == '\r' || *ptr == '\n' || *ptr == ' ' || *ptr == '\t' )
     {
       ptr++;
     }
     else
     {
       break;
     }
   }

   memmove( szDest , ptr , strlen( ptr ) + 1 ) ;

   ptr = szDest;
   nLen = strlen(ptr);
   ptr = szDest + nLen - 1;

   while( ptr >= szDest )
   {
     if( *ptr == '\r' || *ptr == '\n' || *ptr == ' ' || *ptr == '\t' )
     {
       --ptr;
     }
     else
     {
       break;
     }
   }

   *(ptr + 1) = 0 ;

   return szDest;
 }

 return NULL;

}


static inline char BtoA( unsigned char b )
{
  if ( b < 10 )
  {
    return ('0' + b) ;
  }
  if ( b < 16 )
  {
    return (( char ) ('A' + b - 10)) ;
  }

  return '*' ;
}

static inline char AtoB( char c )
{
  if ( (c >= '0') && (c <= '9') )
  {
    return (( char ) (c - '0')) ;
  }
  if ( (c >= 'A') && (c <= 'F') )
  {
    return (( char ) (c - 'A' + 10)) ;
  }
  if ( (c >= 'a') && (c <= 'f') )
  {
    return (( char ) (c - 'a' + 10)) ;
  }
  return 0 ;
}

char *Bin2Str( TMemPool * a , const char *bin , int len )
{
  char *str = MemPoolAllocEx( a , len * 2 + 1 ) ;
  char *ptr = str;

  while ( len > 0 )
  {
    *str = BtoA( ( unsigned char ) ((( unsigned char ) *bin) >> 4) ) ;
    ++str ;
    *str = BtoA( ( unsigned char ) ((( unsigned char ) *bin) & 15) ) ;
    ++str ;
    ++bin ;
    --len ;
  }

  *str = '\0' ;
  return ptr ;
}

void binToStr(const char *bin,char *str,int len)
{
  while ( len > 0 )
  {
    *str = BtoA( ( unsigned char ) ((( unsigned char ) *bin) >> 4) ) ;
    ++str ;
    *str = BtoA( ( unsigned char ) ((( unsigned char ) *bin) & 15) ) ;
    ++str ;
    ++bin ;
    --len ;
  }

  *str = '\0' ;

  return ;
}

char *Str2Bin( TMemPool * a , const char *str , int len )
{
  char *bin = MemPoolAllocEx( a , len + 1 ) ;
  char *ptr = bin;

  while ( len > 0 )
  {
    if ( *str == '\0' )
    {
      return ptr;
    }
    if ( *(str + 1) == '\0' )
    {
      *bin = ( char ) ( unsigned char ) (AtoB( *str ) * 16) ;
      return ptr ;
    } ;
    *bin = ( char ) ( unsigned char ) (AtoB( *str ) * 16 + AtoB( *(str + 1) )) ;
    ++bin ;
    str += 2 ;
    len-- ;
  }

  return ptr;
}

void strToBin( const char *str , char *bin,int len )
{
  while ( len > 0 )
  {
    if ( *str == '\0' )
    {
      return ;
    }
    if ( *(str + 1) == '\0' )
    {
      *bin = ( char ) ( unsigned char ) (AtoB( *str ) * 16) ;
      return  ;
    } ;
    *bin = ( char ) ( unsigned char ) (AtoB( *str ) * 16 + AtoB( *(str + 1) )) ;
    ++bin ;
    str += 2 ;
    len-- ;
  }

  return  ;
}

char *m_strtok(char *str, const char *sep, char **last)
{
  char *token;

  if ( !str )
  {
    str = *last;
  }

  while (*str && strchr(sep, *str))
  {
    ++str;
  }

  if (!*str)
  {
    return NULL;
  }

  token = str;

  *last = token + 1;

  while (**last && !strchr(sep, **last))
  {
    ++*last;
  }

  if (**last)
  {
    **last = '\0';
    ++*last;
  }

  return token;

}

int HexToAsc(char *Hex, char *Asc, int HexLen)
{
  int    i;
  char  *AscPtr;
  char  *HexPtr;
  char   Temp;

  HexPtr = Hex;
  AscPtr = Asc;
  for( i = 0; i < HexLen; i++)
  {
    Temp = (*HexPtr & 0xf0) >> 4;
    if (Temp < 10)
      *AscPtr = 0x30 + Temp;
    else
      *AscPtr = 0x37 + Temp;
    AscPtr++;
    Temp = *HexPtr & 0x0f;
    if (Temp < 10)
      *AscPtr = 0x30 + Temp;
    else
      *AscPtr = 0x37 + Temp;
    AscPtr++;
    HexPtr++;
  }
  return 0;
}

int AscToHex(char *Asc, char *Hex, int AscLen)
{
  char *HexPtr = Hex;
  int   i;

  for(i = 0; i < AscLen; i++)
  {
    *HexPtr = Asc[i] << 4;
    if ( !( Asc[i] >= '0' && Asc[i] <= '9' ) )
      *HexPtr += 0x90;
    i++;
    *HexPtr |= (Asc[i] & 0x0F);
    if (!(Asc[i]>='0' && Asc[i]<='9'))
      *HexPtr += 0x09;
    HexPtr++;
  }
  return 0;
}

int BcdToAscL(char *Bcd, char *Asc, int BcdLen)
{
  int    i;
  char  *BcdPtr = Bcd;
  char  *AscPtr = Asc;

  for(i = 0; i < BcdLen; i++)
  {
    *AscPtr = (*BcdPtr >> 4)&0x0f;
    *AscPtr |= 0x30;
    AscPtr++;
    i++;
    if(i == BcdLen)
    {
      BcdPtr++;
      break;
    }
    *AscPtr = *BcdPtr & 0x0f;
    *AscPtr |= 0x30;
    AscPtr++;
    BcdPtr++;
  }
  return 0;
}

int BcdToAscL2(char *Bcd, char *Asc, int BcdLen)
{
  int    i;
  char  *BcdPtr = Bcd;
  char  *AscPtr = Asc;

  for(i = 0; i < BcdLen; i++)
  {
    *AscPtr = (*BcdPtr >> 4)&0x0f;
    if(*AscPtr <= 0x09)
    {
      *AscPtr |= 0x30;
    }
    else 
    {
      *AscPtr |= 0x40;
      *AscPtr -= 9;
    }
    AscPtr++;
    i++;
    if(i == BcdLen)
    {
      BcdPtr++;
      break;
    }
    *AscPtr = *BcdPtr & 0x0f;
    if(*AscPtr <= 0x09)
    {
      *AscPtr |= 0x30;
    }
    else 
    {
      *AscPtr |= 0x40;
      *AscPtr -= 9;
    }
    AscPtr++;
    BcdPtr++;
  }
  return 0;
}

int BcdToAscR(char *Bcd, char *Asc, int BcdLen)
{
  int   i;
  char *BcdPtr = Bcd;
  char *AscPtr = Asc;

  if(BcdLen%2)
  {
    *AscPtr = *BcdPtr & 0x0f;
    *AscPtr |= 0x30;
    AscPtr++;
    BcdPtr++;
    BcdLen--;
  }

  for(i = 0; i < BcdLen; i++)
  {
    i++;
    *AscPtr = (*BcdPtr >> 4) & 0x0f;
    *AscPtr |= 0x30;
    AscPtr++;

    *AscPtr = *BcdPtr & 0x0F;
    *AscPtr |= 0x30;
    AscPtr++;
    BcdPtr++;
  }
  return 0;
}

int AscToBcdL(char *Asc, char *Bcd, int AscLen)
{
  int  i;
  char *BcdPtr = Bcd;
  char *AscPtr = Asc;

  for( i = 0; i < AscLen; i++)
  {
    *BcdPtr = *AscPtr << 4;
    AscPtr++;
    i++;
    if(i == AscLen)
    {
      *BcdPtr |= 0x0F;
      BcdPtr++;
      break;
    }
    *BcdPtr |= *AscPtr & 0x0F;
    AscPtr++;
    BcdPtr++;
  }
  return 0;
}

int AscToBcdR(char *Asc, char *Bcd, int AscLen)
{
  int   i;
  char *BcdPtr = Bcd;
  char *AscPtr = Asc;

  if(AscLen % 2 )
  {
    *BcdPtr = *AscPtr & 0x0F;
    AscPtr++;
    BcdPtr++;
    AscLen--;
  }

  for(i = 0; i < AscLen; i++)
  {
    i++;
    *BcdPtr = *AscPtr << 4;
    AscPtr++;
    *BcdPtr |= *AscPtr & 0x0F;
    AscPtr++;
    BcdPtr++;
  }
  return 0;
}


int AscToNnn(char *Asc,char **Nnn,int AscLen,char Type)
{
  switch (Type)
  {
    case STRING_TYPE_ASC:
      memcpy(*Nnn,Asc,AscLen);
      *Nnn += AscLen;
      break;
    case STRING_TYPE_LBCD:
      AscToBcdL(Asc,*Nnn,AscLen);
      *Nnn+=(AscLen/2+(AscLen%2));
      break;
    case STRING_TYPE_RBCD:
      AscToBcdR(Asc,*Nnn,AscLen);
      *Nnn+=(AscLen/2+(AscLen%2));
      break;
    case STRING_TYPE_HEX:
      AscToHex(Asc,*Nnn,AscLen);
      *Nnn+=AscLen/2;
      break;
    case STRING_TYPE_ETLBCD:
      AscToBcdL(Asc,*Nnn,AscLen);
      *Nnn+=(AscLen/2+(AscLen%2));
      break;
    default:
      return TTS_EINVAL;
  }
	
  return TTS_SUCCESS;
}

int NnnToAsc(char **Nnn,char *Asc,int AscLen,char Type)
{
  switch (Type)
  {
    case STRING_TYPE_ASC:
      memcpy(Asc,*Nnn,AscLen);
      *Nnn += AscLen;
      break;
    case STRING_TYPE_LBCD:
      BcdToAscL(*Nnn,Asc,AscLen);
      *Nnn+=(AscLen/2+(AscLen%2));
      break;
    case STRING_TYPE_RBCD:
      BcdToAscR(*Nnn,Asc,AscLen);
      *Nnn+=(AscLen/2+(AscLen%2));
      break;
    case STRING_TYPE_HEX:
      HexToAsc(*Nnn,Asc,AscLen/2);
      *Nnn+=AscLen/2;
      break;
    case STRING_TYPE_ETLBCD:
      BcdToAscL2(*Nnn,Asc,AscLen);
      *Nnn+=(AscLen/2+(AscLen%2));
      break;
    default:
      return TTS_EINVAL;
  }
	
  return TTS_SUCCESS;
}
