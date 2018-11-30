#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
typedef struct
{
    int pos;
    int len;
    int size;
    char * data;
} CURL_BUFFER_T;

int32_t curl_get_data(char * url, CURL_BUFFER_T * buffer);
CURL_BUFFER_T * init_curl_buffer(size_t buffer_len);
void uninit_curl_buffer(CURL_BUFFER_T ** buffer);

#ifdef __cplusplus
}
#endif

#endif