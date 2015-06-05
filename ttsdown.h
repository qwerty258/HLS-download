#ifndef _TTSDOWN_H
#define _TTSDOWN_H

/******************************
	M3u8Info: m3u8信息结构体
	@video_num:			视频个数
	@video_url: 		视频下载链接
	@video_encrypt	视频是否加密
*******************************/
typedef struct
{
	int video_num;
	char *video_url[400];
	int video_encrypt;
} M3u8Info;


/******************************
	http_analysis_m3u8: m3u8数据解析
	@m3u8: 				(In)m3u8数据
	@m3u8_len:		(In)m3u8数据长度
	@m3u8info: 		(Out)解析得到的m3u8信息
*******************************/
int http_analysis_m3u8(char *m3u8, int m3u8_len, M3u8Info *m3u8info);


/******************************
	http_download_course: 下载一个课程的视频
	@course: 			(In)课程名
	@time:				(In)时间("am"或者"pm")
*******************************/
int http_download_course(char *course, char *time);

#endif // _TTSDOWN_H
