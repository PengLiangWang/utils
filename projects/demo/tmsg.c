#include "ttsys.h"


int main(int argc, char **argv)
{
   TMessageQueue    queue;
   int              res;
   uint32           key = 0;

   if (argc < 2 )
   {
      printf("tmsg init|open id\n");
      return 0;
   }

   if ( strcmp(argv[1], "init") == 0 )
   {
      res = MessageQueueCreate(&queue, key);
      if (res )
      {
         printf("msgget error %d\n",res);
         return res;
      }
   } 
   else if ( strcmp(argv[1], "open") == 0 )
   {
      if ( argc != 3 )
      {
         printf("tmsg init|open id\n");
         return 0;
      }
      key = (uint32)(atol(argv[2]));
      res = MessageQueueOpenByID(&queue, key);
      if (res )
      {
         printf("msgget error %d\n",res);
         return res;
      }
   }
   printf("qid = %012u\n", queue.qid);
   return 0;
}
