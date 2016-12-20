#include "rdsapi.h"

char s[] = "{\"serv_code\":\"100001\", \"customer_id\":\"A00000001\", \"req_date\":\"20160914\", \"req_time\":\"152405\"}";

int main(int args, char *argv[])
{
	int		ret;
	rds_handler_t	rds;

	if(args != 2)
	{
		fprintf(stderr, "wrong arguments");
		return -1;
	}
	
	rds_handler_init(&rds);

	rds_set_address(&rds, "192.168.1.46", 6379);

	if(strcmp(argv[1], "send") == 0)
	{
		ret = rds_handler_send(&rds, "QBATEST", "20160908123212", s);
		if(ret)
    	{
        	return -1;
    	}
	}

	if(strcmp(argv[1], "recv") == 0)
	{
		char	recv[100] = {'\0'};
		ret = rds_handler_recv_cycle(&rds, "QBATEST", recv, sizeof(recv));
		if(ret == -1)
		{
			printf("recv buffer fail\n");
			return -1;
		}
		if(ret == 1)
		{
			printf("no data recv\n");
			return 0;
		}

		printf("recv buffer success: %s\n", recv);
	}

	return 0;
}

