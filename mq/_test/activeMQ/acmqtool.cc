#include <iostream>
#include <stdio.h>
#include <time.h>

#include "mqhandle.h"

extern "C"
{

int amcq_msg_send(char *brokerURI, char *destURI, char *msg)
{
	//Producer producer(brokerURI, destURI, false);
	MSQProducer sender(brokerURI, destURI, false, false);

	sender.initLib();
	sender.msgsend(msg, time(NULL));
	
	sender.close();

	sender.detachLib();

	return 0;
}

int amcq_msg_recv(char *brokerURI, char *destURI, void **recvmsg)
{
	MSQConsumer recver(brokerURI, destURI, false, false);
	
	recver.initLib();

	recver.msgrecv(NULL);

	recver.getRecvMsg((char **)recvmsg);

	printf("%s\n", (char *)*recvmsg);
	//recver.bufferClear();

	recver.close();
	
	recver.detachLib();

}

/**/
}
/**/


