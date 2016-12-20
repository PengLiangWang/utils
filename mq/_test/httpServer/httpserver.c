#include "httpserver.h"
#include "utils.h"

HttpServer *HttpServer_New()
{
	HttpServer  *http = malloc(sizeof(HttpServer));

	http->httpd = malloc(sizeof(HttpStruct));

	return http;
}

int HttpServer_Listen(HttpServer *httpd, char *host, int port)
{
	int		sockfd;

	if( strcmp(host, "*") == 0){
		strcpy(httpd->host, "0.0.0.0");
	}

	strncpy(httpd->host, host, 19);
	httpd->port = port;

	sockfd = socket_create(host, port);
	if(sockfd == -1){
		fprintf(stderr, "Socket Create Error: %s\n", GetError());
		return -1;
	}

	httpd->sockfd = sockfd;

	printf("Http Listen [%d] connect ok\n", sockfd);
	return 0;
}

int HttpServer_Run(HttpServer *httpd, HandleFunc handle)
{
	int		ret;
	int		connfd;
	char	buffer[1025];
	while(1){
		connfd = handle_accpet(httpd->sockfd);
		if(connfd == -1){
			fprintf(stderr, "Accept Fail: %s\n", GetError());
			continue;
		}

		memset(buffer, 0, sizeof(buffer));

		ret = handle_recv(connfd, buffer);
		if(ret == -1){
			fprintf(stderr, "Recieve Fail: %s\n", GetError());
			close(connfd);
			continue;
		}
	
		printf("%s\n", buffer);
		
		memset(buffer, 0, sizeof(buffer));
		strcat(buffer, "HTTP/1.1 200 OK\r\n");
		//strcpy(buffer, "HTTP/1.1 200 OK\r\n");
		//handle_send(connfd, buffer);
		//strcat(buffer, "Content-Type: text/plain\r\n");
		strcat(buffer, "Content-Type: text/plain; charset=utf-8\r\n");
		//strcpy(buffer, "Content-Type: text/plain; charset=utf-8\r\n");
		//handle_send(connfd, buffer);
		//sprintf(buffer+strlen(buffer), "Content-Length: %d\r\n", 100);
		strcat(buffer, "Hello World!\n");
		//strcpy(buffer, "Hello World!\r\n");
		
		handle_send(connfd, buffer);
		sleep(1);
		close(connfd);
	}

	return 0;
}
