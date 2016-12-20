#include "httpserver.h"
#include <unistd.h>
#include <stdlib.h>

void *httphandle(HttpServer *httpd)
{
	printf("handle http server\n");	
	return (void *)0;
}

int main()
{
	int		ret;
	HttpServer	*httpd = HttpServer_New();

	printf("New HttpServer\n");

	ret = HttpServer_Listen(httpd, "192.168.1.46", 8081);
	if(ret){
		exit(1);
	}

	printf("HttpServer Listen\n");

	HttpServer_Run(httpd, httphandle);

	exit(0);
}
