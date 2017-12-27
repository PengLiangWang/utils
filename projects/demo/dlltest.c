
#include "ttsys.h"

static TMemPool  *MemPool = NULL;

void ExitClear()
{  
   if ( MemPool )
   {
      MemPoolDestoryEx(MemPool);
   }
}

int main(int argc, char **argv)
{
   int         res;
   TDllHandle *handle;
   void       *sym;

   if ( argc != 3 )
   {
      printf("dlltest filename.so  soname\n");
      exit(-1);   
   }

   atexit(ExitClear);

   res = MemPoolCreateEx(&MemPool, NULL );
   if ( res )
   {
      printf("创建全局内存池失败,错误信息(%d,%s)\n",res,GetStrError(res));
      exit(-1); 
   }

   res = DllLoad(&handle,(const char *)argv[1], MemPool);
   if ( res )
   {
      printf("打开动态库(%s)失败,错误信息(%d,%s)\n",argv[1],res,GetStrError(res));
      exit(-1); 
   }
   //TEngineModule
   res = DllSym((TDllHandleSym *)&module, handle, (const char *)vmethod->name);
   if ( res )
   {
      printf("打开动态库(%s)失败,错误信息(%d,%s)\n",argv[1],res,GetStrError(res));
      exit(-1); 
   }
   
}
