typedef struct
{
  int   a;
  int   b;
}Tab;


int main()
{
   Tab   *tabs;
   Tab    taba[10];
   int    i;
   Tab  **tbs;

   tabs = taba;
   tbs = (Tab **)&taba;

   for ( i = 0 ;i < 10;i++)
   {
      //printf("tabs %X\n", tabs++);
      printf("tabs %X | %X\n", &taba[i], tabs++);
   }

   printf("=========\n");
   for ( i = 0 ;i < 10;i++)
   {
      tabs = (tbs[i]);
      printf("tbs %X\n", tabs);
   }

   if ( 1 > 1 )
   {
      printf("......\n");
   }
};
