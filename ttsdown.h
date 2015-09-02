/*******************************************************************
 *  Copyright(c) 2015 ruanyu
 *  All rights reserved.
 *
 *  创建日期: 2015-07-25
 *  修改日期: 2015-07-25
 *  作者: ruanyu
 ******************************************************************/

#ifndef _TTSDOWN_H
#define _TTSDOWN_H

typedef struct
{
    int video_num;
    char *video_url[400];
    int video_encrypt;
} M3u8Info;

int http_analysis_m3u8(char *m3u8, int m3u8_len, M3u8Info *m3u8info);

int http_download_course(char *course, char *time);

#endif // _TTSDOWN_H
