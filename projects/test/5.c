#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
   char    *fmt="QUEUE/FML32/1223";
   int      res,iLen;
   char     qName[32], qType[32];

   memset(qName, 0, sizeof(qName));
   memset(qType, 0, sizeof(qType));
   iLen = 0;
   res = sscanf(fmt, "%[^/]/%[^/]/%d", qName, qType,&iLen);
   printf("res = %d , (%s) (%s) (%s) (%d)\n", res, qName, qType, fmt, iLen);
   return 0;
}
