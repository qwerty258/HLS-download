#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/aes.h>

#include "http.h"
#include "utils.h"

#define UserAgent "User-Agent: ttsdownload/1.2 (Linux i686)"
#define Referer "Referer: http://tts6.tarena.com.cn/scripts/bofang/StrobeMediaPlayback.swf"
#define Host "Host: videotts.it211.com.cn"
#define Connection "Connection: keep-alive"

char *http_download(const char * url, int *content_len)
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
	sprintf(http_request, "GET %s HTTP/1.1\r\n%s\r\n%s\r\n%s\r\n%s\r\n\r\n", url, UserAgent, Referer, Host, Connection);
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

int http_analysis_m3u8(char *m3u8, int m3u8_len, M3u8Info *m3u8info)
{
	if(!m3u8 || m3u8_len <= 0 || !m3u8info)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	if(memstr(m3u8, m3u8_len, "AES") != NULL)
	{
		m3u8info->video_encrypt = 1;
	}
	else
	{
		m3u8info->video_encrypt = 0;
	}

	int cnt = 0;
	char *m3u8_line = strtok(m3u8, "\n");
	if(!m3u8_line)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	while(m3u8_line)
	{
		if(!memcmp(m3u8_line, "http", 4))
		{
			m3u8info->video_url[cnt++] = m3u8_line;
		}
		m3u8_line = strtok(NULL, "\n");
	}
	m3u8info->video_num = cnt;
	return 0;
}

int http_download_course(char *course, char *time)
{
	if(!course || !time)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	char down_url[100];
	sprintf(down_url, "%s/%s%s/%s%s.m3u8", "http://videotts.it211.com.cn", course, time, course, time);
	int down_m3u8_len = 0;
	char *down_m3u8 = http_download(down_url, &down_m3u8_len);
	if(!down_m3u8)
	{
		return -1;
	}

	M3u8Info down_m3u8info;
	if(http_analysis_m3u8(down_m3u8, down_m3u8_len, &down_m3u8info) == -1)
	{
		free(down_m3u8);
		return -1;
	}

	sprintf(down_url, "%s/%s%s/static.key", "http://videotts.it211.com.cn", course, time);
	int down_key_len = 0;
	char *down_key = http_download(down_url, &down_key_len);
	if(!down_key)
	{
		free(down_m3u8);
		return -1;
	}

	char file_path[FILENAME_MAX];
	sprintf(file_path, "./%s/%s%s.ts", course, course, time);
	FILE *fp = fopen(file_path, "wb");
	if(!fp)
	{
		printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
		free(down_m3u8);
		free(down_key);
		return -1;
	}

	for(int i = 0; i < down_m3u8info.video_num; i++)
	{
		int down_ts_len = 0;
		char *down_ts = http_download(down_m3u8info.video_url[i], &down_ts_len);
		if(down_m3u8info.video_encrypt == 1)
		{
			AES_KEY aes_key;
			unsigned char aes_iv[16];
			memset(aes_iv, '\0', 16);
			if(AES_set_decrypt_key((unsigned char *)down_key, 128, &aes_key) < 0)
			{
				printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
				free(down_m3u8);
				free(down_key);
				free(down_ts);
				return -1;
			}
			char *aes_ts = malloc(down_ts_len);
			AES_cbc_encrypt((unsigned char *)down_ts, (unsigned char *)aes_ts, down_ts_len, &aes_key, aes_iv, AES_DECRYPT);
			free(down_ts);
			down_ts = aes_ts;
		}
		if(fwrite(down_ts, 1, down_ts_len, fp) != down_ts_len)
		{
			printf("Error:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
			free(down_m3u8);
			free(down_key);
			free(down_ts);
			return -1;
		}
		free(down_ts);

		printf("\rprogress(%s%s):%d/%d", course, time, i + 1, down_m3u8info.video_num);
		fflush(stdout);
	}

	printf("\n");
	char mkvtool_cmd[200];
	sprintf(mkvtool_cmd, "mkvmerge -o ./%s/%s%s.mkv --forced-track 0:no --forced-track 1:no -a 1 -d 0 -S -T --no-global-tags --no-chapters %s --track-order 0:0,0:1", course, course, time, file_path);
	system(mkvtool_cmd);
	remove(file_path);
	free(down_m3u8);
	free(down_key);
	fclose(fp);
	return 0;
}
