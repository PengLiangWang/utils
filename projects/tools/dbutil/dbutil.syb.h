#ifndef _DB_INCLUDE_H_
#define _DB_INCLUDE_H_

#define SQLNOTFOUND 100
#define USESYBASE
#ifdef SYBASE_CP
EXEC SQL BEGIN DECLARE SECTION;
typedef long	int32;
typedef struct
{
	char	cursor_name[16];
} Select_Info;
typedef Select_Info Lock_Info;
EXEC SQL END DECLARE SECTION;
#else

#ifdef	__cplusplus
extern "C" {
#endif

typedef long	int32;
typedef struct
{
	char	cursor_name[16];
} Select_Info;
typedef Select_Info Lock_Info;

#ifdef	__cplusplus
}
#endif

#endif

#ifdef	__cplusplus
extern "C" {
#endif

int dbOpen(char *dbName,char *dbPasswd);
int dbWaitLock(int waitTime);
int dbNoWaitLock();
int dbClose();
int dbBeginWork();
int dbCommit();
int dbRollback();
int dbGetNewCursorName(char *cursName);
int dbFreeCursName(char *name);
int dbCloseSelect(Select_Info *select_info);
int dbFreeLock(Lock_Info *lock_info);
int dbKillSpace(char *str,int strSize);

#define KILL_SPACE(str)		dbKillSpace(str,sizeof(str))
#define ERASE_TAIL_SPACE(str)	KILL_SPACE(str)

#define DB_NOT_FOUND	SQLNOTFOUND
#define DB_DUP_RECORD	-100

#define DT_ASC 1						     /* ASCII */
#define DT_NUM 2					    /* ORACLE NUMERIC */
#define DT_ITG 3						   /* INTEGER */
#define DT_REA 4						      /* REAL */
#define DT_STR 1						    /* STRING */
#define DT_RAW 6				       /* BINARY DATA STRINGS */
#define DT_LNG 8						      /* Long */

#define	DT_DAT	128					/* DATE */
#define	DT_YMD	129					/* YYYY-MM-DD */
#define	DT_YM	130					/* YYYY-MM    */
#define DT_TIM	131					/* HH:MM:SS YJS */

#ifdef	__cplusplus
}
#endif

#endif
