/*
 This Include File "DB_serial.h" 
 Genenated By
 Application dbaction V3.0 for Oracle
 with the action file "DB_serial.act".
 Create: Mon Oct 15 14:24:29 2012
*/

#ifndef _DB_sys_serial_INCLUDE_H_
#define _DB_sys_serial_INCLUDE_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "dbutil.h"

typedef struct {
	int	id;
	long	serial_no;
        } Sys_Serial;

int DB_sys_serial_read_lock_by_id( int id__0,Sys_Serial *_a_data,Lock_Info *_a_lock);
int DB_sys_serial_update_by_id( int id__0,Sys_Serial *_a_data);
int DB_sys_serial_free_lock( Lock_Info *_a_lock);
int DB_sys_serial_add( Sys_Serial *_a_data);
int DB_sys_serial_del_by_id( int id__0);
int DB_sys_serial_print_report(FILE *out);

#ifdef	__cplusplus
}
#endif


#endif
