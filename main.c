/*******************************************************************
 *  Copyright(c) 2015 ruanyu
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

#ifdef _MSC_VER

#include <WinSock2.h>
#include <Windows.h>
void WSAInit(void)
{
    WSADATA WSAData;
    if(0 != WSAStartup(MAKEWORD(2, 2), &WSAData))
    {
        printf("WSAStartup error: %d\n", WSAGetLastError());
    }
}
void WSAFree(void)
{
    if(0 != WSACleanup())
    {
        printf("WSACleanup error: %d\n", WSAGetLastError());
    }
}

#endif // _MSC_VER


int main(int argc, char *argv[])
{
#ifdef _MSC_VER
    WSAInit();
#endif // _MSC_VER
    if(argc != 4)
    {
        printf("ttsdownload\n\tusage: ttsdownload <course> <set_date> <end_date>\n");
#ifdef _MSC_VER
        WSAFree();
#endif // _MSC_VER
        return -1;
    }
    int set_date, end_date;
    set_date = atoi(argv[2]);
    end_date = atoi(argv[3]);
    if(!set_date || !end_date)
    {
        printf("ttsdownload\n\tusage: ttsdownload <course> <set_date> <end_date>\n");
#ifdef _MSC_VER
        WSAFree();
#endif // _MSC_VER
        return -1;
    }
    while(set_date <= end_date)
    {
        char course[20];
        sprintf(course, "%s%04d", argv[1], set_date);
#ifdef _MSC_VER
        CreateDirectoryA(course, NULL);
#else
        mkdir(course, 0777);
#endif // _MSC_VER
        int res[2];
        res[0] = http_download_course(course, "am");
        res[1] = http_download_course(course, "pm");
        if((res[0] == -1) && (res[1] == -1))
        {
            remove(course);
        }
        set_date++;
    }
#ifdef _MSC_VER
    WSAFree();
#endif // _MSC_VER
    return 0;
}

