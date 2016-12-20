#include <stdio.h>

int main()
{
	char	*recv;
	amcq_msg_recv("failover://(tcp://192.168.1.29:61616)", "TEST.FOO", (void **)&recv);

	printf("recv: %s\n", recv);
	
	return 0;
}
