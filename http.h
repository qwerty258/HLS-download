#ifndef _http_h
#define _http_h

typedef struct
{
	int status;// 响应状态
	int content_len;// 文件总长度
	char *body;// 响应包中文件数据的指针
	int body_len;// 响应包中文件数据的长度
} RespondInfo;

typedef struct
{
	int video_num;// 视频个数(最多200)
	char *video_url[200];// 视频链接(最多200)
	int video_encrypt;// 是否加密
} M3u8Info;

char *http_download(const char *url, int *content_len);
int http_analysis_host(const char *url);
int http_analysis_respond(const char *respond, int respond_len, RespondInfo *respondinfo);
int http_analysis_m3u8(char *m3u8, int m3u8_len, M3u8Info *m3u8info);
int http_download_course(char *course, char *time);

#endif // _http_h
