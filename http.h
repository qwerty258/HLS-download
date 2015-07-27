/*******************************************************************
 *  Copyright(c) 2015 Company Name
 *  All rights reserved.
 *
 *  创建日期: 2015-07-25
 *  修改日期: 2015-07-25
 *  作者: ruanyu
 ******************************************************************/

#ifndef _HTTP_H
#define _HTTP_H

typedef struct
{
	int status;
	int content_len;
	char *body;
	int body_len;
} RespondInfo;

char *http_download(const char *url, int *content_len, const char *referer);

int http_analysis_host(const char *url);

int http_analysis_respond(const char *respond, int respond_len, RespondInfo *respondinfo);

#endif // _HTTP_H
