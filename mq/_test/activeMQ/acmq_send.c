#include <stdio.h>

int main()
{
	amcq_msg_send("failover://(tcp://192.168.1.29:61616)", "TEST.FOO", "I Love You");

	return 0;	
}
