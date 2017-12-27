#ifndef _DB_TUOTUO_SERIAL_INCLUDE_H_
#define _DB_TUOTUO_SERIAL_INCLUDE_H_

#ifdef   __cplusplus
extern "C" {
#endif

#include "dbutil.h"

typedef struct 
{
   char  serialName[33];
   long  serialNo;
   long  maxSerialNo;
} MySerial;

int DB_MySerial_read_lock_by_name( char *table,char *name__0,MySerial *_a_data,Lock_Info *_a_lock);
int DB_MySerial_update_by_name( char *table,char *name__0,MySerial *_a_data);
int DB_MySerial_free_lock( Lock_Info *_a_lock);
int DB_MySerial_add( char *table,MySerial *_a_data);

#ifdef   __cplusplus
}
#endif


#endif
