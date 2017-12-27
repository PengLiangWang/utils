#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IB.h"
#include "opt_data.h"
#include "logger.h"

int main()
{
  RCI_ENV       rci_env;
  OPT_RCV_DATA  sndData; 
  OPT_SND_DATA  rcvData;
  int           rv;
  extern char  *CurrentProgram;

  CurrentProgram = "RCI_CLIENT";

  memset(&rci_env,0,sizeof(rci_env));
  memset(&rcvData,0,sizeof(rcvData));
  memset(&sndData,0,sizeof(sndData));
  
  strcpy(rci_env.message_type,"0820");
  strcpy(rci_env.trans_code,"000000");
  strcpy(rci_env.remote_node_id,"09610000");
  sndData.number = 100;
  strcpy(sndData.name,"tomszhou");
  sndData.age = 99;
  strcpy(sndData.address,"nanjinghuadian");

  printf("snd.number  = %d\n",sndData.number);
  printf("snd.name    = %s\n",sndData.name  );
  printf("snd.age     = %d\n",sndData.age   );
  printf("snd.address = %s\n",sndData.address);

  rv = IB_RCI_CALL(&rci_env,&sndData,&rcvData,&IB_RCI_PKG_OPT_RCV_DATA,
                  &IB_RCI_PKG_OPT_SND_DATA,NULL,NULL);
  if(rv)
  {
    printf("F[%s] L[%d] IB_RCI_CALL err %d\n",__FILE__,__LINE__,rv);
    return rv;
  }

  printf("rcv.retcode = %d\n",rcvData.retcode);
  printf("snd.amout   = %.3f\n",rcvData.amount );

  printf("ok!!!\n");
  return 0;
}
