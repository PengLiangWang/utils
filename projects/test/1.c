#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define DLOG_PREFIX   __FUNCTION__,__FILE__,__LINE__

void dlog(const char *fn,const char *fl,const int line,char *fmt, ...)
{
   va_list args;

   va_start(args, fmt);

   printf("%s %s %d \n",fn,fl,line);

   vprintf(fmt,args);

   va_end(args);
}

#define DLOG(...)  dlog(DLOG_PREFIX ,__VA_ARGS__)

int main()
{
  DLOG((char *)"FFFFFF %d\n",100);
  dlog(__FUNCTION__,__FILE__,__LINE__,"FFFF %d\n",100);
  return 0;
}
