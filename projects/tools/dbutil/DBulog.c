#include "DBulog.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>



static void _dblog(const char *file,const int line,char *fmt,va_list ap)
{
   FILE      *fp;
   char      *p;
   char       fileName[ 256 ];
   time_t     tv;
   struct tm *local_t;

   time (&tv);
   local_t=localtime(&tv);
   
   if ( ( p = (char *)getenv("HOME") ) == NULL )
   {
      return ;
   }

   snprintf(fileName,256,"%s/log/DB%04d%02d%02d.log",p,local_t->tm_year+1900,local_t->tm_mon+1,local_t->tm_mday);
   
   fp = fopen ( fileName,"a");
   if ( fp == NULL )
   {
      return ;
   }

   fprintf(fp, "[%02d:%02d:%02d] (%d) (%s:%d) ",local_t->tm_hour,local_t->tm_min,local_t->tm_sec,(int)getpid(),file ,line );
   vfprintf(fp , fmt, ap);
   fprintf(fp, "\n");
   fclose ( fp );
   
   return ;

}

void DBlog_print(const char *file,const int line,char *fmt,...)
{
   va_list  args;

   va_start(args,fmt);
   _dblog(file,line,fmt,args);
   va_end(args);
   return ;
}



