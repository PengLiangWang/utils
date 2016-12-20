#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct http_struct_s
{
	char	method[20];
	char	content_type[26];
	char	connection[20];
	char	accpet_lang[13];
	char	accept_encode[41];
	char	*get_params;
} HttpStruct;

typedef struct http_server_s
{
	char		host[20];
	int			port;
	int			sockfd;
	HttpStruct	*httpd;
} HttpServer;

typedef void *(*HandleFunc)(HttpServer *httpd);

#endif
