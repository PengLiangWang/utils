#ifndef _DBUTIL_INCLUDE_H_
#define _DBUTIL_INCLUDE_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "OracleOci.h"

#define USEORACLE
extern DataBaseServer	dbServer;
extern unsigned long	db_row_process_count;

typedef DBcurs	Select_Info;
typedef DBcurs	Lock_Info;

typedef DBcurs	TLockInfo;
typedef DBcurs	TSelectInfo;

#define dbCloseSelect(select_info)	dbCursClose(select_info);
#define dbFreeLock(lock_info)		dbCursClose(lock_info);

int dbOpen(char *user,char *passwd,char *dbname);
int dbClose();
int dbBeginWork();
int dbRollback();
int dbCommit();
int dbWaitLock(int waitTime);
int dbNoWaitLock();
int dbGetErrMsg(char *errmsg,int size);

int dbExecSql(char *sqlCmd);
int dbExecSql_va(char *sqlCmd,...);
/*
	use:
		dbExecSql_va(char *sqlcmd ,
				[ char *valueName ,
				  char *valueP,
				  int  valueSize,
				  int  valueType,
				]
				...
				NULL);
*/

int dbCursOpen(DBcurs *curs);
int dbCursClose(DBcurs *curs);
int dbCursDefineSqlCmd(DBcurs *curs,char *sqlCmd);
int dbCursDefineValue(DBcurs *curs,
			char *valueName,char *valueP,int size,int type);
int dbCursDefineRecvValue(DBcurs *curs,
			int valueN,char *valueP,int size,int type);
int dbCursDefineRecvValueAndRCode(DBcurs *curs,
			int valueN,char *valueP,int size,int type,
			int *rWide,int *rCode);
int dbCursGetRecvValueInfo(DBcurs *curs,int valueN,
		long *dbsize,short *dbtype,char *name,long *nameSize,
		long *dsize,short *perc,short *scale,short *nullok);
int dbCursExec(DBcurs *curs);
int dbCursFetch(DBcurs *curs);
int dbCursStopFetch(DBcurs *curs);
int dbCursGetErrMsg(DBcurs *curs,char *errmsg,int size);
int dbCursGetRowID(DBcurs *curs,char *RowID,int size);


int dbCursExecSql_va(DBcurs *curs,char *sqlCmd,...);
/*
	use:
		dbCursOpen(DBcurs *curs);
		dbCursExecSql_va(DBcurs *curs,
			  char *sqlcmd ,
				[ char *valueName ,
				  char *valueP,
				  int  valueSize,
				  int  valueType,
				]
				...
				NULL);
		dbCursClose(DBcurs *curs);

*/

int dbCursDefineSql_va(DBcurs *curs,char *sqlCmd,...);
/*
	use:
		dbCursOpen(DBcurs *curs);
		dbCursDefineSql_va(
				DBcurs *curs,
			  	char *sqlcmd ,
				[ char *valueName ,
				  char *valueP,
				  int  valueSize,
				  int  valueType,
				]
				...
				NULL);
		dbCursExec(DBcurs *curs);
		dbCursClose(DBcurs *curs);
*/
int dbCursDefineSelect_va(DBcurs *curs,char *selectCmd,...);
/*
	use:
		dbCursOpen(DBcurs *curs);
		dbCursDefineSelect_va(
				DBcurs *curs,
			  	char *sqlcmd ,
				[
				  char *returnValueP,
				  int returnValueSize,
				  int returnValueType,
				]
				...
				NULL,
				[
				  char *valueName ,
				  char *valueP,
				  int  valueSize,
				  int  valueType,
				]
				...
				NULL);
		dbCursExec(DBcurs *curs);

		dbCursFetch(DBcurs *curs);
		...

		dbCursClose(DBcurs *curs);
*/

int dbKillSpace(char *str,int size);
#define KILL_SPACE(name)	dbKillSpace(name,sizeof(name))
#define ERASE_TAIL_SPACE(name)	KILL_SPACE(name)

#ifdef	__cplusplus
}
#endif

#endif
