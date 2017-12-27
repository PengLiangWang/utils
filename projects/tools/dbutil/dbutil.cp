#include <stdio.h>
#include "businessdb.h"
#include "dbutil.h"

void  error_handler();
void  warning_handler();
EXEC SQL INCLUDE sqlca;
#define SQLCODE   sqlca.sqlcode
EXEC SQL WHENEVER SQLERROR CALL error_handler();
EXEC SQL WHENEVER SQLWARNING CALL warning_handler();
EXEC SQL WHENEVER NOT FOUND CONTINUE;

typedef struct	NameList_t
	{
		struct	NameList_t	*next;
		int			stat;
		char			name[16];
	}NameList;

static NameList		*nList=NULL;

int dbFreeCursName(char *name)
{
	NameList	*p;

	p=nList;
	while(p!=NULL)
		if (strcmp(p->name,name)==0)
		{
			p->stat=0;
			return(0);
		}
		else p=p->next;
	return(-1);
}

int dbGetNewCursorName(char *cursName)
{
	static	int	n=0;
	NameList	*p;

	p=nList;
	while(p!=NULL)
		if (p->stat==0)
		{
			strcpy(cursName,p->name);
			p->stat=1;
			return(0);
		}
		else p=p->next;

	sprintf(cursName,"CUR%d",++n);
	p=(NameList *)malloc(sizeof(NameList));
	if (p==NULL)
		return(-1);
	memset(p,0,sizeof(NameList));
	p->next=nList;
	nList=p;
	p->stat=1;
	strcpy(p->name,cursName);

/**
	sprintf(cursName,"CUR%X",cursName);
**/
	return(0);
}

int dbOpen(char *dbName,char *dbPasswd)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char *username;
		char *userpasswd;
		char *name;
	EXEC SQL END DECLARE SECTION;

	username = dbName;
	userpasswd = dbPasswd;

	EXEC SQL CONNECT :username IDENTIFIED BY :userpasswd;
	if (SQLCODE != 0){
		IBcomlog_print(__FILE__,__LINE__,"Connect error:%d", SQLCODE);
		return(SQLCODE);
	}

	name = getenv(BUSINESS_DB);
	if(name == NULL){
		IBcomlog_print(__FILE__,__LINE__,
			"Envirnment %s missed\n", BUSINESS_DB);
		return(-1);
	}

	EXEC SQL USE :name;
	if (SQLCODE != 0)
		IBcomlog_print(__FILE__,__LINE__,"USE database[%s] error:%d", name, SQLCODE);
#if ERRORLOG
if(SQLCODE)
errlog("open datebase error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
}

int dbWaitLock(int waitTime)
{
/*
	switch(waitTime)
	{
		case 10:	EXEC SQL SET LOCK MODE TO WAIT 10;
				break;
		case 50:	EXEC SQL SET LOCK MODE TO WAIT 50;
				break;
		case 100:	EXEC SQL SET LOCK MODE TO WAIT 100;
				break;
		default:
			EXEC SQL SET LOCK MODE TO WAIT ;
			break;
	};
#if ERRORLOG
if(SQLCODE)
errlog("waitlock error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
*/
	return(0);
}
int dbNoWaitLock()
{
/*
	EXEC SQL SET LOCK MODE TO NOT WAIT;
#if ERRORLOG
if(SQLCODE)
errlog("nowaitlock error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
*/
	return(0);
}

int dbClose()
{
	EXEC SQL DISCONNECT DEFAULT;

#if ERRORLOG
if(SQLCODE)
errlog("close datebase error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
}

int dbBeginWork()
{
	EXEC SQL BEGIN TRANSACTION;
#if ERRORLOG
if(SQLCODE)
errlog("begin work error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
}

int dbRollback()
{
	EXEC SQL ROLLBACK TRANSACTION;
#if ERRORLOG
if(SQLCODE)
errlog("rollback error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
}

int dbCommit()
{
	EXEC SQL COMMIT TRANSACTION;
#if ERRORLOG
if(SQLCODE)
errlog("commit error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
}

int dbCloseSelect(Select_Info *select_info)
{
	
	EXEC SQL BEGIN DECLARE SECTION;
		char	*curs;
	EXEC SQL END DECLARE SECTION;

	curs=select_info->cursor_name;
	EXEC SQL CLOSE  :curs;

	dbFreeCursName(select_info->cursor_name);
	select_info->cursor_name[0]='\0';
#if ERRORLOG
if(SQLCODE)
errlog("closeselect error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
}

int dbFreeLock(Lock_Info *lock_info)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	*curs;
	EXEC SQL END DECLARE SECTION;

	curs=lock_info->cursor_name;
	EXEC SQL CLOSE :curs;
	dbFreeCursName(lock_info->cursor_name);
	lock_info->cursor_name[0]='\0';
#if ERRORLOG
if(SQLCODE)
errlog("freelock error code=%d",SQLCODE);
#endif
	return(sqlca.sqlcode);
}


int dbKillSpace(char *str,int size)
{
	int	len;

	len=strlen(str);
	if (len<size)
		memset(str+len,0,size-len);
	while((--len)>=0)
		if (str[len]==' ')
			str[len]='\0';
		else return(len+1);
	return(len+1);
}


void error_handler()
{

	if (sqlca.sqlerrm.sqlerrml){
		IBcomlog_print(__FILE__,__LINE__, "\n** %s sqlcode[%d]", 
					sqlca.sqlerrm.sqlerrmc, sqlca.sqlcode);
	}

}

/*
** void warning_handler()
** 
**	Displays warning messages.
*/
void warning_handler()
{

	if (sqlca.sqlwarn[1] == 'W'){
		IBcomlog_print(__FILE__,__LINE__, "\n** Data truncated.\n");
	}

	if (sqlca.sqlwarn[3] == 'W'){
		IBcomlog_print(__FILE__, __LINE__,
			"\n** Insufficient host variables to store results.\n");
	}	
	return;
}

