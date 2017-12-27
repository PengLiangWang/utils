#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IB.h"
#include "opt_data.h"
#include "logger.h"

int main()
{
  RCI_ENV       rci_env;
  OPT_RCV_DATA  rcvData; 
  OPT_SND_DATA  sndData;
  int           rv;
  extern char  *CurrentProgram;

  CurrentProgram = "RCI_SERVER";

  memset(&rcvData,0,sizeof(rcvData));
  memset(&sndData,0,sizeof(sndData));
  
  printf("start ..................\n");
  rv = IB_RCI_INIT(&rci_env,&rcvData,&IB_RCI_PKG_OPT_RCV_DATA,NULL);
  if(rv)
  {
    printf("F[%s] L[%d] IB_RCI_INIT err %d\n",__FILE__,__LINE__,rv);
    return rv;
  }

  printf("rcv.number  = %d\n",rcvData.number);
  printf("rcv.name    = %s\n",rcvData.name  );
  printf("rcv.age     = %d\n",rcvData.age   );
  printf("rcv.address = %s\n",rcvData.address);

  sndData.retcode = 109;
  sndData.amount  = 1983.9870;

  rv = IB_RCI_RETURN(&rci_env,&sndData,&IB_RCI_PKG_OPT_SND_DATA,NULL);
  if(rv)
  {
    printf("F[%s] L[%d] IB_RCI_RETURN err %d\n",__FILE__,__LINE__,rv);
    return rv;
  }
  printf("ok!!!");
  return 0;
}
