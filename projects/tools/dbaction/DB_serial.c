/* 
 This C File "DB_serial.c" 
 Genenated By
 Application dbaction V3.0 for Oracle
 with the action file "DB_serial.act".
 Create: Mon Oct 15 14:24:29 2012
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "DBulog.h"
#include "DB_serial.h"
static Sys_Serial	R_sys_serial;

static int Sys_Serial_EraseTailSpace(Sys_Serial *_erase_data)
{
	return(0);
}
int DB_sys_serial_read_lock_by_id( int id__0,Sys_Serial *_a_data,Lock_Info *_a_lock)
{
        int     r;

   memset(&R_sys_serial,0,sizeof(Sys_Serial));
        r=dbCursOpen(_a_lock);
        if (r!=0)
                return(r);
        r=dbCursDefineSelect_va(_a_lock,
                "SELECT  \n\
			 id \n\
			,serial_no \
		FROM sys_serial \
		WHERE id =:id__0 \
		FOR UPDATE WAIT 25",
		&R_sys_serial.id,4,DT_ITG,
		&R_sys_serial.serial_no,4,DT_ITG,
		NULL,
		":id__0",&id__0,4,DT_ITG,
		NULL);
        if (r!=0)
        {
                goto E;
        };
        r=dbCursExec(_a_lock);
        if (r!=0)
        {
                goto E;
        };
        r=dbCursFetch(_a_lock);
        if (r==SQLNOTFOUND) goto E;
        if (r!=0)
        {
                goto E;
        };


	Sys_Serial_EraseTailSpace(&R_sys_serial);
        memcpy(_a_data,&R_sys_serial,sizeof(Sys_Serial));

  E:
        if (r!=0) dbCursClose(_a_lock);
        return(r);
}

int DB_sys_serial_update_by_id( int id__0,Sys_Serial *_a_data)
{
        int     r;
        memcpy(&R_sys_serial,_a_data,sizeof(Sys_Serial));
        r=dbExecSql_va("UPDATE sys_serial \
			SET  \
			 id=:U_id \
			,serial_no=:U_serial_no\
		WHERE id =:id__0",
		":U_id",&R_sys_serial.id,4,DT_ITG,
		":U_serial_no",&R_sys_serial.serial_no,4,DT_ITG,
		":id__0",&id__0,4,DT_ITG,
		NULL);
	if (r==0&&db_row_process_count==0)
		r=SQLNOTFOUND;
        return(r);
}

int DB_sys_serial_free_lock( Lock_Info *_a_lock)
{
        int     r;
        r=dbCursClose(_a_lock);
        return(r);
}

int DB_sys_serial_add( Sys_Serial *_a_data)
{
        int     r;
        memcpy(&R_sys_serial,_a_data,sizeof(Sys_Serial));
        r=dbExecSql_va("INSERT INTO sys_serial \
		( id \n\
		,serial_no \n\
		) \
		VALUES ( \n\
			 :U_id \n\
			,:U_serial_no)",
		":U_id",&R_sys_serial.id,4,DT_ITG,
		":U_serial_no",&R_sys_serial.serial_no,4,DT_ITG,
		NULL);
        return(r);
}

int DB_sys_serial_del_by_id( int id__0)
{
        int     r;
        r=dbExecSql_va("DELETE FROM sys_serial \
		WHERE id =:id__0",
		":id__0",&id__0,4,DT_ITG,
		NULL);
	if (r==0&&db_row_process_count==0)
		r=SQLNOTFOUND;
        return(r);
}

int DB_sys_serial_print_report(FILE *out)
{

	int r,i;
	Select_Info aInfo;
	Sys_Serial P_sys_serial;

	r = DB_sys_serial_open_select(&aInfo);
	if (r!=0) 
		return(r);
	fprintf(out, "\n%s\t%s\n","id" ,"serial_no" );
	for (i=0; i<80; i++)
		fprintf(out, "-");

	while ((r = DB_sys_serial_fetch_select(&aInfo, &P_sys_serial) == 0)){
	fprintf(out, "\n%d\t%ld",P_sys_serial.id ,P_sys_serial.serial_no );
	}
	if (r!=SQLNOTFOUND)
		return(r);
	r = DB_sys_serial_close_select(&aInfo);
	if (r!=0) 
		return(r);
	return(0);
}

int DB_sys_serial_debug_print(char *reason,Sys_Serial *adata, char *filename, int line_no)
{
	DBlog_print(filename,line_no,"TABLE [Sys_Serial] REASON[%s] LOG",reason);

	DBlog_print(filename,line_no, "id: %d", adata->id );
	DBlog_print(filename,line_no, "serial_no: %ld", adata->serial_no );

	return(0);
}
