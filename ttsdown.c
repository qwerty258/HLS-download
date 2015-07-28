/*******************************************************************
 *  Copyright(c) 2015 ruanyu
 *  All rights reserved.
 *
 *  创建日期: 2015-07-25
 *  修改日期: 2015-07-25
 *  作者: ruanyu
 ******************************************************************/
#include "config.h"
#ifdef HAVE_LARGE_FILE
#undef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif // HAVE_LARGE_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/aes.h>

#include "http.h"
#include "utils.h"
#include "ttsdown.h"

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
	const char *down_referer = "User-Agent: ttsdownload/1.2 (Linux i686)\r\nReferer: http://tts6.tarena.com.cn/scripts/bofang/StrobeMediaPlayback.swf\r\nHost: videotts.it211.com.cn\r\nConnection: keep-alive";
	sprintf(down_url, "%s/%s%s/%s%s.m3u8", "http://videotts.it211.com.cn", course, time, course, time);
	int down_m3u8_len = 0;
	char *down_m3u8 = http_download(down_url, &down_m3u8_len, down_referer);
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
	char *down_key = http_download(down_url, &down_key_len, down_referer);
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
		char *down_ts = http_download(down_m3u8info.video_url[i], &down_ts_len, down_referer);
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
#ifdef HAVE_MKV_PACK
	char mkvtool_cmd[200];
	sprintf(mkvtool_cmd, "mkvmerge -o ./%s/%s%s.mkv --forced-track 0:no --forced-track 1:no -a 1 -d 0 -S -T --no-global-tags --no-chapters %s --track-order 0:0,0:1", course, course, time, file_path);
	system(mkvtool_cmd);
#endif // HAVE_MKV_PACK
	remove(file_path);
	free(down_m3u8);
	free(down_key);
	fclose(fp);
	return 0;
}
