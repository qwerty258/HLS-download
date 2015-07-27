/*******************************************************************
 *  Copyright(c) 2015 Company Name
 *  All rights reserved.
 *
 *  创建日期: 2015-07-25
 *  修改日期: 2015-07-25
 *  作者: ruanyu
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "http.h"
#include "ttsdown.h"

int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		printf("ttsdownload\n\tusage: ttsdownload <course> <set_date> <end_date>\n");
		return -1;
	}

	int set_date, end_date;
	set_date = atoi(argv[2]);
	end_date = atoi(argv[3]);
	if(!set_date || !end_date)
	{
		printf("ttsdownload\n\tusage: ttsdownload <course> <set_date> <end_date>\n");
		return -1;
	}

	while(set_date <= end_date)
	{
		char course[20];
		sprintf(course, "%s%04d", argv[1], set_date);
		mkdir(course, 0777);

		int res[2];
		res[0] = http_download_course(course, "am");
		res[1] = http_download_course(course, "pm");

		if((res[0] == -1) && (res[1] == -1))
		{
			remove(course);
		}
		set_date++;
	}
	return 0;
}

