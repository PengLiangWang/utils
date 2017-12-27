#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ttdbserial.h"
#include "elog.h"

static MySerial   R_MySerial;

static int MySerial_EraseTailSpace(MySerial *_erase_data)
{
   ERASE_TAIL_SPACE(_erase_data->serialName);
   return 0;
}

int DB_Sequence_read_by_name( char *name__0,MySerial *_a_data)
{
   int     res;
   char    SQL[512];

   memset(SQL,0,sizeof(SQL));
   sprintf(SQL,"SELECT %s.nextval FROM DUAL",  name);
   memset(&R_MySerial,0,sizeof(MySerial));
   res = dbCursOpen(_a_lock);
   if ( res != 0 )
   {
      return res;
   }
   res = dbCursDefineSelect_va(_a_lock,SQL,R_MySerial.serialName,32,DT_STR,
                               &R_MySerial.serialNo,4,DT_ITG,
                               &R_MySerial.maxSerialNo,4,DT_ITG,
                               NULL,
                               ":name__0",name__0,-1,DT_STR,
                               NULL);
   if ( res != 0 )
   {
      goto E;
   }

   res = dbCursExec(_a_lock);
   if ( res != 0 )
   {
      goto E;
   }
   
   res = dbCursFetch(_a_lock);
   if ( res != 0 )
   {
      goto E;
   }
   
   MySerial_EraseTailSpace(&R_MySerial);
   
   memcpy(_a_data,&R_MySerial,sizeof(MySerial));

E:
   if ( res != 0 )
   {
      dbCursClose(_a_lock);
   }

   return res;
}

int DB_MySerial_update_by_name( char *table, char *name__0,MySerial *_a_data)
{

   int     res;
   char    SQL[512];

   memset(SQL,0,sizeof(SQL));
   sprintf(SQL,"UPDATE %s SET serialNo =:U_logNo WHERE serialName =:name__0 ",table);
   memcpy(&R_MySerial,_a_data,sizeof(MySerial));
   res = dbExecSql_va(SQL,":U_logNo",&R_MySerial.serialNo,4,DT_ITG,
                                     ":name__0",name__0,-1,DT_STR,
                                     NULL);
   if ( res == 0 && db_row_process_count == 0 )
   {
      res = SQLNOTFOUND;
   }

   return res;
}

int DB_MySerial_free_lock( Lock_Info *_a_lock)
{
   int   res;

   res = dbCursClose(_a_lock);

   return res;
}

int DB_MySerial_add( char *table,MySerial *_a_data)
{
   int  res;
   char SQL[512];

   memset(SQL,0,sizeof(SQL));
   sprintf(SQL,"INSERT INTO %s ( serialName,serialNo,maxSerialNo ) VALUES ( :U_name,:U_logNo,:U_maxLogNo) ",table);
   memcpy(&R_MySerial,_a_data,sizeof(MySerial));

   res = dbExecSql_va(SQL,":U_name",R_MySerial.serialName,-1,DT_STR,
                          ":U_logNo",&R_MySerial.serialNo,4,DT_ITG,
                          ":U_maxLogNo",&R_MySerial.maxSerialNo,4,DT_ITG,
                          NULL);
   return res;
}

