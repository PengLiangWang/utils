#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "dso.h"

typedef void *            TOsDllHandle;

struct TDllHandle
{
   TMemPool     *pool;
   void         *handle;
   const char   *errmsg;
};

int DllHandlePut(TDllHandle **dso, TOsDllHandle osdso,TMemPool *pool)
{
   *dso = MemPoolAllocEx(pool, sizeof **dso);
   (*dso)->handle = osdso;
   (*dso)->pool = pool;
   return TTS_SUCCESS;
}

int OsDllHandleGet(TOsDllHandle *osdso,TDllHandle *aprdso)
{
   *osdso = aprdso->handle;
   return TTS_SUCCESS;
}

static int DllCleanup(void *thedso)
{
   TDllHandle *dso = thedso;

   dlclose(dso->handle);

   dso->handle = NULL;

   return TTS_SUCCESS;
}


int DllLoad(TDllHandle **res_handle,const char *path, TMemPool *pool)
{
  void *os_handle = dlopen((char *)path, RTLD_NOW | RTLD_GLOBAL);

  *res_handle = MemPoolAllocEx(pool, sizeof(**res_handle));

  if( os_handle == NULL )
  {
    (*res_handle)->errmsg = dlerror();
    return TTS_DLOPEN;
  }

  (*res_handle)->handle = (void*)os_handle;
  (*res_handle)->pool = pool;
  (*res_handle)->errmsg = NULL;

  MemPoolRegisterCleanup(pool,*res_handle,DllCleanup);
  
  return TTS_SUCCESS;
}

int DllUnload(TDllHandle *handle)
{
  return MemPoolRunCleanup(handle->pool, handle, DllCleanup);
}

int DllSym(TDllHandleSym *ressym, TDllHandle *handle, const char *symname)
{
  void *retval = dlsym(handle->handle, symname);  

  if (retval == NULL)
  {
    handle->errmsg = dlerror();
    return TTS_NOTFOUND;
  }

  *ressym = retval;
  return TTS_SUCCESS;
}

const char * DllError(TDllHandle *dso, char *buffer, uint32 buflen)
{
  if (dso->errmsg) 
  {
    m_cpystrn(buffer, dso->errmsg, buflen);
    return dso->errmsg;
  }
  return "success";
}
