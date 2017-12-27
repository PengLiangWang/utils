#ifndef _HSM_MODULE_H_
#define _HSM_MODULE_H_

struct _hsm_module_
{
  int                numOfCmd;
  THsmCommandTable  *table;
};

typedef struct _hsm_module_ THsmModule;


#endif

