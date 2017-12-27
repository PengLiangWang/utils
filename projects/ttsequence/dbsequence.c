#include <string.h>
#include <stdio.h>

#include "dbutil.h"

int DB_Sequence_read_by_name(char *seqName,long *sequence)
{
   int         r;
   DBcurs      _a_curs;
   char        sql[128];
   long        tmpSeq;

   r = dbCursOpen(&_a_curs);
   if ( r )
   {
      return r;
   }

   memset( sql, 0, sizeof(sql) );

   sprintf(sql, "SELECT  %s.nextval FROM DUAL", seqName);

   r = dbCursDefineSelect_va(&_a_curs, sql, &tmpSeq ,4,DT_ITG, NULL, NULL);
   if ( r )
   {
      dbCursClose(&_a_curs);
      return r;
   }

   r = dbCursExec(&_a_curs);
   if ( r )
   {
      dbCursClose(&_a_curs);
      return r;
   }

   r = dbCursFetch(&_a_curs);
   if ( r )
   {
      dbCursClose(&_a_curs);
      return r;
   }

   dbCursClose(&_a_curs);

   *sequence = tmpSeq;

   return r;
}


int main()
{
   int   r ;
   long  current;
   char  name[32];

   r = BusinessDataBaseOpen();
   if ( r )
   {
      printf("打开数据库失败,%d\n",r);
   }

   memset(name, 0, sizeof(name) );
   strcpy(name,"CUSTOMERID");
   current = 0;
   
   r =  DB_Sequence_read_by_name(name, &current);
   if (r )
   {
      printf("获取ＤＵＡＬ 失败, EEEEEEEEEE    %d\n",r);
   }
   else
   {
      printf("name %s = %d\n",name, current);
   }

   BusinessDataBaseClose();

   return 0;
}
