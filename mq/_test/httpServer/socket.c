#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <errno.h>
#include "utils.h"

int errno;

int socket_create(char *host, int port)
{
	int			ret;
	int			sockfd;
	struct sockaddr_in	server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd <= 0){
		SetError(strerror(errno));
		return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(host);

	ret = bind(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr));
	if( ret ){
		SetError(strerror(errno));
		close(sockfd);
		return -1;
	}

	int optval = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if(ret){
		SetError(strerror(errno));
		close(sockfd);
		return -1;
	}

	ret = listen(sockfd, 10);
	if(ret){
		SetError(strerror(errno));
		close(sockfd);
		return -1;
	}

	return sockfd;
}

int handle_accpet(int sockfd)
{
	int   		connfd;
	struct 		sockaddr_in  client;
	socklen_t	len = sizeof(client);

	connfd = accept(sockfd, (struct sockaddr *)&client, &len);
	if(connfd == -1){
		SetError(strerror(errno));
		return -1;
	}

	return connfd;
}

int handle_recv(int connfd, char *buffer)
{
	int		size;

	size = recv(connfd, buffer, 1024, 0);
	if(size == -1){
		SetError(strerror(errno));
		return -1;
	}

	return 0;
}

int handle_send(int connfd, char *buffer)
{
	int		ret;

	printf("send: %s\n", buffer);
	ret = send(connfd, buffer, strlen(buffer), 0);
	if(ret == -1){
		fprintf(stderr, "Fail to Send:%s\n", GetError(errno));
		return -1;
	}

	return 0;
}

