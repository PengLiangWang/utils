#ifndef _DB_UGC_LOG_H_
#define _DB_UGC_LOG_H_

#ifdef	__cplusplus
extern "C" {
#endif

void DBlog_print(const char *file,const int line,char *fmt,...);

#ifdef	__cplusplus
}
#endif

#define DBLOG_PREFIX  __FILE__,__LINE__

#define DBLOG(...)  DBlog_print(DBLOG_PREFIX,__VA_ARGS__)

#endif


