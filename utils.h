#ifndef _utils_h
#define _utils_h

/******************************
	memstr: 非0结尾数据中搜索字符串
	@full_data:				数据
	@full_data_len: 	数据长度
	@substr:					要搜索的字符串
*******************************/
char* memstr(const char* full_data, int full_data_len, const char* substr);

#endif // _utils_h
