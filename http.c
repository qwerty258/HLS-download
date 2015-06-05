#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "http.h"
#include "utils.h"

char *http_download(const char *url, int *content_len, const char *referer)
{
	if(!url || !content_len)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return NULL;
	}

	int http_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(http_socket == -1)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return NULL;
	}

	int host = 0;
	if((host = http_analysis_host(url)) == -1)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		close(http_socket);
		return NULL;
	}

	struct sockaddr_in http_sockaddr;
	http_sockaddr.sin_addr.s_addr = host;
	http_sockaddr.sin_family = AF_INET;
	http_sockaddr.sin_port = htons(80);
	memset(http_sockaddr.sin_zero, '\0', 8);

	if(connect(http_socket, (struct sockaddr *)&http_sockaddr, sizeof(http_sockaddr)) == -1)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		close(http_socket);
		return NULL;
	}

	char http_request[512];
	sprintf(http_request, "GET %s HTTP/1.1\r\n%s\r\n\r\n", url, referer);
	if(send(http_socket, http_request, strlen(http_request), MSG_NOSIGNAL) <= 0)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		close(http_socket);
		return NULL;
	}
#ifdef _DEBUG
	printf("http_request_size=%d\n", strlen(http_request));
#endif // _DEBUG
	int http_respond_len;
	char http_respond[2048];
	if((http_respond_len = recv(http_socket, http_respond, 2048, 0)) <= 0)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		close(http_socket);
		return NULL;
	}

	RespondInfo respondinfo;
	if(http_analysis_respond(http_respond, http_respond_len, &respondinfo) == -1)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		close(http_socket);
		return NULL;
	}

	if(respondinfo.status != 200)
	{
		printf("respond %d\n", respondinfo.status);
		close(http_socket);
		return NULL;
	}

	char *content = malloc(respondinfo.content_len);
	memcpy(content, respondinfo.body, respondinfo.body_len);
	*content_len = respondinfo.body_len;
	while(*content_len != respondinfo.content_len)
	{
		if((http_respond_len = recv(http_socket, http_respond, 2048, 0)) <= 0)
		{
			printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
			close(http_socket);
			return NULL;
		}
		memcpy(content + *content_len, http_respond, http_respond_len);
		*content_len += http_respond_len;
	}
	close(http_socket);
	return content;
}

int http_analysis_host(const char *url)
{
	if(!url)
	{
		return -1;
	}

	char host[100];
	struct hostent *host_ent;

	char *host_set = strstr(url, "//");
	if(!host_set)
	{
		return -1;
	}
	host_set += 2;

	char *host_end = strstr(host_set, "/");
	if(!host_end)
	{
		strcpy(host, host_set);
		host_ent = gethostbyname(host);
	}
	else
	{
		memcpy(host, host_set, host_end - host_set);
		host[host_end - host_set] = '\0';
		host_ent = gethostbyname(host);
	}
	if(!host_ent)
	{
		return -1;
	}
	return ((struct in_addr *)(host_ent->h_addr_list[0]))->s_addr;
}

int http_analysis_respond(const char *respond, int respond_len, RespondInfo *respondinfo)
{
	if(!respond || respond_len <= 0 || !respondinfo)
	{
		return -1;
	}
	char *respnod_body = memstr(respond, respond_len, "\r\n\r\n") + 4;
	if(!respnod_body)
	{
		return -1;
	}

	respondinfo->body = respnod_body;
	respondinfo->body_len = respond_len - (respnod_body - respond);

	char respnod_temp[respond_len];
	memcpy(respnod_temp, respond, respond_len);

	char *respnod_line = strtok(respnod_temp, "\r\n");
	if(!respnod_line)
	{
		return -1;
	}
	while(respnod_line)
	{
		if(!strncmp(respnod_line, "HTTP/1.1", strlen("HTTP/1.1")))
		{
			if((respondinfo->status = atoi(respnod_line + strlen("HTTP/1.1 "))) == 0)
			{
				return -1;
			}
		}
		if(!strncmp(respnod_line, "Content-Length:", strlen("Content-Length:")))
		{
			if((respondinfo->content_len = atoi(respnod_line + strlen("Content-Length: "))) == 0)
			{
				return -1;
			}
		}
		respnod_line = strtok(NULL, "\r\n");
	}
	return 0;
}
