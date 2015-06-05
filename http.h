#ifndef _http_h
#define _http_h

/******************************
	RespondInfo: 响应信息结构体
	@status:				响应状态
	@content_len: 	文件总长度
	@body:					响应包中文件数据的指针
	@body_len:			响应包中文件数据的长度
*******************************/
typedef struct
{
	int status;
	int content_len;
	char *body;
	int body_len;
} RespondInfo;


/******************************
	http_download: Http文件下载
	@url: 				(In)要下载的链接
	@content_len: (Out)实际下载的长度
	@referer: 		(In)Http请求附加信息
*******************************/
char *http_download(const char *url, int *content_len, const char *referer);


/******************************
	http_analysis_host: 域名Dns解析
	@url: 				(In)要下载的链接
*******************************/
int http_analysis_host(const char *url);


/******************************
	http_analysis_respond: Http响应包解析
	@respond: 		(In)响应包数据
	@respond_len:	(In)响应包数据长度
	@respondinfo: (Out)解析得到的响应信息
*******************************/
int http_analysis_respond(const char *respond, int respond_len, RespondInfo *respondinfo);

#endif // _http_h
