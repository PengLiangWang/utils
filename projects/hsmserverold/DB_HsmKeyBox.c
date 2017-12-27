/* 
 This C File "DB_HsmKeyBox.c" 
 Genenated By
 Application dbaction V3.0 for Oracle
 with the action file "DB_HsmKeyBox.act".
 Create: Fri Aug 17 13:33:54 2012
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "DBulog.h"
#include "DB_HsmKeyBox.h"
static HsmKeyBoxEx	R_HsmKeyBoxEx;

static int HsmKeyBoxEx_EraseTailSpace(HsmKeyBoxEx *_erase_data)
{
	ERASE_TAIL_SPACE(_erase_data->keyType);
	ERASE_TAIL_SPACE(_erase_data->keyFlag);
	ERASE_TAIL_SPACE(_erase_data->keyDataA);
	ERASE_TAIL_SPACE(_erase_data->keyDataB);
	ERASE_TAIL_SPACE(_erase_data->keyDac);
	return(0);
}
int DB_HsmKeyBoxEx_read_by_keyIndex( int keyIndex__0,HsmKeyBoxEx *_a_data)
{
        int     r;
        DBcurs  _a_curs;

	memset(&R_HsmKeyBoxEx,0,sizeof(HsmKeyBoxEx));
        r=dbCursOpen(&_a_curs);
        if (r!=0)
                return(r);
        r=dbCursDefineSelect_va(&_a_curs,
                "SELECT  \n\
			 keyIndex \n\
			,keyType \n\
			,keyFlag \n\
			,keyLen \n\
			,keyDataA \n\
			,keyDataB \n\
			,keyDac \
		FROM HsmKeyBoxEx \
		WHERE keyIndex =:keyIndex__0",
		&R_HsmKeyBoxEx.keyIndex,4,DT_ITG,
		R_HsmKeyBoxEx.keyType,1,DT_STR,
		R_HsmKeyBoxEx.keyFlag,1,DT_STR,
		&R_HsmKeyBoxEx.keyLen,4,DT_ITG,
		R_HsmKeyBoxEx.keyDataA,256,DT_STR,
		R_HsmKeyBoxEx.keyDataB,256,DT_STR,
		R_HsmKeyBoxEx.keyDac,32,DT_STR,
		NULL,
		":keyIndex__0",&keyIndex__0,4,DT_ITG,
		NULL);
        if (r!=0)
        {
                goto E;
        };
        r=dbCursExec(&_a_curs);
        if (r!=0)
        {
                goto E;
        };
        r=dbCursFetch(&_a_curs);
        if (r==SQLNOTFOUND) goto E;
        if (r!=0)
        {
                goto E;
        };


	HsmKeyBoxEx_EraseTailSpace(&R_HsmKeyBoxEx);
        memcpy(_a_data,&R_HsmKeyBoxEx,sizeof(HsmKeyBoxEx));

  E:
        dbCursClose(&_a_curs);
        return(r);
}

int DB_HsmKeyBoxEx_update_by_keyIndex( int keyIndex__0,HsmKeyBoxEx *_a_data)
{
        int     r;
        memcpy(&R_HsmKeyBoxEx,_a_data,sizeof(HsmKeyBoxEx));
        r=dbExecSql_va("UPDATE HsmKeyBoxEx \
			SET  \
			 keyIndex=:U_keyIndex \
			,keyType=:U_keyType \
			,keyFlag=:U_keyFlag \
			,keyLen=:U_keyLen \
			,keyDataA=:U_keyDataA \
			,keyDataB=:U_keyDataB \
			,keyDac=:U_keyDac\
		WHERE keyIndex =:keyIndex__0",
		":U_keyIndex",&R_HsmKeyBoxEx.keyIndex,4,DT_ITG,
		":U_keyType",R_HsmKeyBoxEx.keyType,-1,DT_STR,
		":U_keyFlag",R_HsmKeyBoxEx.keyFlag,-1,DT_STR,
		":U_keyLen",&R_HsmKeyBoxEx.keyLen,4,DT_ITG,
		":U_keyDataA",R_HsmKeyBoxEx.keyDataA,-1,DT_STR,
		":U_keyDataB",R_HsmKeyBoxEx.keyDataB,-1,DT_STR,
		":U_keyDac",R_HsmKeyBoxEx.keyDac,-1,DT_STR,
		":keyIndex__0",&keyIndex__0,4,DT_ITG,
		NULL);
	if (r==0&&db_row_process_count==0)
		r=SQLNOTFOUND;
        return(r);
}

int DB_HsmKeyBoxEx_add( HsmKeyBoxEx *_a_data)
{
        int     r;
        memcpy(&R_HsmKeyBoxEx,_a_data,sizeof(HsmKeyBoxEx));
        r=dbExecSql_va("INSERT INTO HsmKeyBoxEx \
		( keyIndex \n\
		,keyType \n\
		,keyFlag \n\
		,keyLen \n\
		,keyDataA \n\
		,keyDataB \n\
		,keyDac \n\
		) \
		VALUES ( \n\
			 :U_keyIndex \n\
			,:U_keyType \n\
			,:U_keyFlag \n\
			,:U_keyLen \n\
			,:U_keyDataA \n\
			,:U_keyDataB \n\
			,:U_keyDac)",
		":U_keyIndex",&R_HsmKeyBoxEx.keyIndex,4,DT_ITG,
		":U_keyType",R_HsmKeyBoxEx.keyType,-1,DT_STR,
		":U_keyFlag",R_HsmKeyBoxEx.keyFlag,-1,DT_STR,
		":U_keyLen",&R_HsmKeyBoxEx.keyLen,4,DT_ITG,
		":U_keyDataA",R_HsmKeyBoxEx.keyDataA,-1,DT_STR,
		":U_keyDataB",R_HsmKeyBoxEx.keyDataB,-1,DT_STR,
		":U_keyDac",R_HsmKeyBoxEx.keyDac,-1,DT_STR,
		NULL);
        return(r);
}

int DB_HsmKeyBoxEx_open_select( Select_Info *_a_sInfo)
{
        int     r;

        r=dbCursOpen(_a_sInfo);
        if (r!=0)
                return(r);
        r=dbCursDefineSelect_va(_a_sInfo,
                "SELECT  \n\
			 keyIndex \n\
			,keyType \n\
			,keyFlag \n\
			,keyLen \n\
			,keyDataA \n\
			,keyDataB \n\
			,keyDac \
		FROM HsmKeyBoxEx ",
		&R_HsmKeyBoxEx.keyIndex,4,DT_ITG,
		R_HsmKeyBoxEx.keyType,1,DT_STR,
		R_HsmKeyBoxEx.keyFlag,1,DT_STR,
		&R_HsmKeyBoxEx.keyLen,4,DT_ITG,
		R_HsmKeyBoxEx.keyDataA,256,DT_STR,
		R_HsmKeyBoxEx.keyDataB,256,DT_STR,
		R_HsmKeyBoxEx.keyDac,32,DT_STR,
		NULL,
		NULL);
        if (r!=0)
        {
                goto E;
        };
        r=dbCursExec(_a_sInfo);

  E:
        if (r!=0) dbCursClose(_a_sInfo);
        return(r);
}

int DB_HsmKeyBoxEx_fetch_select( Select_Info *_a_sInfo,HsmKeyBoxEx *_a_data)
{
        int     r;

	memset(&R_HsmKeyBoxEx,0,sizeof(HsmKeyBoxEx));
        r=dbCursFetch(_a_sInfo);
        if (r==SQLNOTFOUND) goto E;
        if (r!=0)
        {
                goto E;
        };


	HsmKeyBoxEx_EraseTailSpace(&R_HsmKeyBoxEx);
        memcpy(_a_data,&R_HsmKeyBoxEx,sizeof(HsmKeyBoxEx));

  E:
        return(r);
}

int DB_HsmKeyBoxEx_close_select( Select_Info *_a_sInfo)
{
        int     r;
        r=dbCursClose(_a_sInfo);
        return(r);
}


int DB_HsmKeyBoxEx_debug_print(char *reason,HsmKeyBoxEx *adata, char *filename, int line_no)
{
	DBlog_print(filename,line_no,"TABLE [HsmKeyBoxEx] REASON[%s] LOG",reason);

	DBlog_print(filename,line_no, "keyIndex: %d", adata->keyIndex );
	DBlog_print(filename,line_no, "keyType: %s", adata->keyType );
	DBlog_print(filename,line_no, "keyFlag: %s", adata->keyFlag );
	DBlog_print(filename,line_no, "keyLen: %d", adata->keyLen );
	DBlog_print(filename,line_no, "keyDataA: %s", adata->keyDataA );
	DBlog_print(filename,line_no, "keyDataB: %s", adata->keyDataB );
	DBlog_print(filename,line_no, "keyDac: %s", adata->keyDac );

	return(0);
}
