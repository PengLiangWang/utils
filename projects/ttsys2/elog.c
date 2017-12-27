


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "strings.h"
#include "elog.h"

static int  elogtag = 0;

struct _t_elog_data
{
   int    opend;        /**< 是否打开         */
   int    lmask;        /**< 日志记录方式     */
   int    level;        /**< 日志记录级别     */
   int    connected;    /**< 日志是否连接远程 */
   FILE  *flog;         /**< 日志文件         */
   int    dlog;         /**< 远程句柄         */
   char  *path;         /**< 路径             */
   char  *file;         /**< 文件             */
   char  *module;       /**< 模块             */
   char  *host;         /**< 主机名称         */
   int    port;         /**< PORT             */
   long   ymd;          /**< 计时器           */
   struct tm   *lt;
};


typedef struct _t_elog_data  TELogData;

#define ELOG_DATA_INIT {-1, LOG_FILE_MASK, LOG_LEVEL_INFO, 0, (FILE *)0, -1, (char *)0, (char *)0, (char *)0, (char *)0, 0, 0, (struct tm*)0}


static TELogData  edata = ELOG_DATA_INIT;

//192.168.6.100:1203

static void velog(int pri,const char *function, const char *file,const int line,char *fmt,va_list ap);
static void eloggettime();
static void openelog();
static void elogexit();
static void elogregister();
static void connectelog();

static void elogexit()
{
   if ( edata.flog != NULL )
   {
      fflush(edata.flog);
      fclose(edata.flog);
      edata.flog = NULL;
   }

   if ( edata.module != NULL )
   {
      free(edata.module);
      edata.module = NULL;
   }

   if ( edata.path != NULL )
   {
      free(edata.path);
      edata.path = NULL;
   }

   if ( edata.file != NULL )
   {
      free(edata.file);
      edata.file = NULL;
   }

   if ( edata.host != NULL )
   {
      free(edata.host);
      edata.host = NULL;
   }

   if ( edata.dlog > 0 )
   {
      close(edata.dlog);
      edata.dlog = -1;
      edata.connected = 0;
   }

   edata.opend = -1;
}

static void elogregister()
{
   if ( elogtag )
   {
      return ;
   }
   atexit(elogexit);
   elogtag = 1;
   return ;
}


void eloglevel(int pri)
{
   edata.level = pri;
}

void elogmask(int mask)
{
   edata.lmask = mask;
}

int elogisdebug()
{
  return edata.level == LOG_LEVEL_DEBUG;
}

void eloghost(char *host)
{
   if ( host )
   {
      char  *shost;
      char  *item;
      char  *last;
      int    port;

      shost = strdup(host);
      item = m_strtok(shost,":",&last);
      if ( item == NULL )
      {
         free(shost);
         return ;
      }
      edata.host = strdup(item);

      item = m_strtok(NULL,":",&last);
      if ( item == NULL )
      {
         free(shost);
         return ;
      }
      edata.port = atoi(item);
      free(shost);
   }
   return ;
}

void eloginit(int pri,char *module,char *path,char *file,int lmask,char *host)
{
   if ( pri > LOG_LEVEL_DEBUG || pri < LOG_LEVEL_ERROR )
   {
      edata.level = LOG_LEVEL_ERROR;
   }
   else
   {
      edata.level = pri;
   }

   if ( module )
   {
      edata.module = strdup(module);
   }

   if ( path && *path != 0 && strcmp(path, "NULL") )
   {
      edata.path = strdup(path);
   }

   if ( file )
   {
      edata.file = strdup(file);
   }

   if ( host && *host != 0 && strcmp(host, "NULL") )
   {
      eloghost(host);
   }

   edata.lmask = lmask;

}

void elog(int pri,const char *function, const char *file,const int line,char *fmt,...)
{
   if ( pri > LOG_LEVEL_DEBUG || pri < LOG_LEVEL_ERROR )
   {
      pri = LOG_LEVEL_ERROR;
   }

   if ( pri > edata.level )
   {
      return ;
   }

   elogregister();

   eloggettime();

   {
      va_list args;
      va_start(args, fmt);
      velog(pri, function, file, line, fmt, args);
      va_end(args);
   }

   return ;
}

#define DEC(p,xlen,llen)             \
      do                             \
      {                              \
            if (xlen < 0)            \
                  xlen = 0;          \
            if (xlen >= llen)        \
                  xlen = llen - 1;   \
            p += xlen;               \
            llen -= xlen;            \
      } while (0)



static char   fileline[1024];

static char  *elevel[] = 
{
   "ERROR",
   "WARN ",
   "INFO ",
   "DEBUG"
};

static void velog(int pri,const char *function,const char *file,const int line,char *fmt,va_list ap)
{
   char    *p;
   int      llen,xlen;

   memset(fileline,0,sizeof(fileline));

   llen = sizeof(fileline);
   xlen = 0;
   p = fileline;

   xlen = snprintf(p, llen, "[%04d-%02d-%02d %02d:%02d:%02d] %s ", edata.lt->tm_year+1900,
                   edata.lt->tm_mon+1, edata.lt->tm_mday, edata.lt->tm_hour, edata.lt->tm_min, 
                   edata.lt->tm_sec, elevel[pri]);
   DEC(p,xlen,llen);

   xlen = snprintf(p, llen, "(%s:%d) %s (%s:%d) ", ( edata.module != NULL ) ? edata.module : "unkown", 
                      (int)getpid(), function, file, line);
   DEC(p,xlen,llen);

   xlen = vsnprintf(p, llen, fmt, ap);
   DEC(p,xlen,llen);

   xlen = p - fileline;
   while (fileline[xlen-1] == '\n')
   {
      xlen --;
   }
   fileline[xlen] = '\0';

   if ( edata.lmask & LOG_MASK_TAG )
   {
      openelog();
      if ( edata.opend <= 0 )
      {
         return ;
      }
      
      fprintf(edata.flog,"%s\n",fileline);
      fflush(edata.flog);
   }
   
   if ( ( edata.lmask >> 1 ) & LOG_MASK_TAG )
   {
      connectelog();
      if ( !edata.connected )
      {
         return ;
      }
      send(edata.dlog,fileline,xlen,0);
   }

   return ;
}

static void eloggettime()
{
   static time_t       timer;
   static long         ymd;

   time(&timer);

   edata.lt = (struct  tm  *)localtime(&timer);
   
   ymd = (edata.lt->tm_mon + 1)*100 + edata.lt->tm_mday;
   
   if ( ymd != edata.ymd )
   {
      edata.ymd = ymd;
      if ( edata.flog != NULL )
      {
         fflush(edata.flog);
         fclose(edata.flog);
         edata.opend = -1;
         edata.flog = NULL;
      }
   }
}

static void openelog()
{
   char    logfile[128];
   int     llen ;

   if ( edata.opend > 0 )
   {
      return ;
   }

   llen = sizeof(logfile);

   snprintf(logfile, llen, "%s/log/%s", (char *)getenv("HOME"), ( edata.path != NULL ) ? edata.path : "");
   
   if ( logfile[strlen(logfile) - 1] != '/' )
   {
      logfile[strlen(logfile)] = '/';
   }
   llen -= strlen(logfile);

   if ( access(logfile, 0) != 0)
   {
      if ( mkdir(logfile, 0755) != 0 )
      {
         fprintf(stderr,"can't create logger dir(%s).\n",logfile);
         return ;
      }
   }
   snprintf(logfile + strlen(logfile), llen, "%s.%04d%02d%02d", ( edata.file != NULL ) ? edata.file : "LOG",edata.lt->tm_year+1900, edata.lt->tm_mon+1, edata.lt->tm_mday);
   edata.flog = fopen(logfile,"a+");
   if ( edata.flog == NULL )
   {
      fprintf(stderr,"can't create logger file(%s).\n",logfile);
      return ;
   }

   edata.opend = 1;

   return ;
}

FILE *eloggetfilebyname(char *filename)
{
   static  FILE *fp = NULL;
   char    logfile[128];
   int     llen ;

   eloggettime();

   llen = sizeof(logfile);

   snprintf(logfile, llen, "%s/log/%s", (char *)getenv("HOME"), ( edata.path != NULL ) ? edata.path : "");
   
   if ( logfile[strlen(logfile) - 1] != '/' )
   {
      logfile[strlen(logfile)] = '/';
   }
   llen -= strlen(logfile);

   if ( access(logfile, 0) != 0)
   {
      if ( mkdir(logfile, 0755) != 0 )
      {
         fprintf(stderr,"can't create logger dir(%s).\n",logfile);
         fp = stdout;
         return fp;
      }
   }

   snprintf(logfile + strlen(logfile), llen, "%s.%04d%02d%02d", ( filename != NULL ) ? filename : "LOG",edata.lt->tm_year+1900, edata.lt->tm_mon+1, edata.lt->tm_mday);
   fp = fopen(logfile,"a+");
   if ( fp == NULL )
   {
      fprintf(stderr,"can't create logger file(%s).\n",logfile);
      fp = stdout;
      return fp;
   }

   return fp;

}

#include <netdb.h>
#include <netinet/in.h>

static void connectelog()
{
   struct sockaddr_in  client;

   if ( edata.connected )
   {
      return ;
   }

   if ( edata.host == NULL || edata.port == 0 )
   {
      return ;
   }

   edata.dlog = socket(AF_INET,SOCK_DGRAM,0);
   if ( edata.dlog < 0 )
   {
      fprintf(stderr,"can't create log socket.\n");
      return ;
   }

   memset(&client, 0, sizeof(client));
   client.sin_family = AF_INET;
   client.sin_port = htons(edata.port);
   client.sin_addr.s_addr = inet_addr(edata.host);

   if(connect(edata.dlog,(struct sockaddr *)&client,sizeof(client)) < 0)
   {
      close(edata.dlog);
      edata.dlog = -1;
      fprintf(stderr,"can't connect %s:%d logger server.\n",edata.host,edata.port);
      return ;
   }

   edata.connected = 1;

   return ;
}

