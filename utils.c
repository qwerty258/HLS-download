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

const char* memstr(const char* full_data, int full_data_len, const char* substr)
{
    if(full_data == NULL || full_data_len <= 0 || substr == NULL)
    {
        return NULL;
    }
    if(*substr == '\0')
    {
        return NULL;
    }
    int sublen = strlen(substr); // this is a BUG, if there is 0x00 in subbin, the substr is incorrect!!!
    int i;
    const char* cur = full_data;
    int last_possible = full_data_len - sublen + 1;
    for(i = 0; i < last_possible; i++)
    {
        if(*cur == *substr)
        {
            if(memcmp(cur, substr, sublen) == 0)
            {
                return cur;
            }
        }
        cur++;
    }
    return NULL;
}

