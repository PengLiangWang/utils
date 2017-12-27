
#include "ttsys.h"


int main(int argc, char **argv)
{
   char   key[9];
   char   data1[9];
   char   data2[9];

   memset(data1, 0, sizeof(data1));
   memset(data2, 0, sizeof(data2));
   memset(key  , 0, sizeof(key  ));

   strncpy(key, "01234567", 8);
   strncpy(data1, "11111111", 8);

   endes(key, data1);

   printf("data1 = %s\n",data1);

   dedes(key, data1);

   printf("data2 = %s\n",data1);
   
   return 0;
}
