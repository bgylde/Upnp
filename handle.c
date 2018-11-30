#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "handle.h"

#define DEFAULT_CURL_TIMEOUT 2000

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

CURL_BUFFER_T * init_curl_buffer(size_t buffer_len)
{
    CURL_BUFFER_T * buffer = NULL;

    buffer = (CURL_BUFFER_T *) malloc(sizeof(CURL_BUFFER_T));
    if (buffer == NULL)
    {
        printf("Malloc error!\n");
        return NULL;
    }

    memset(buffer, 0, sizeof(CURL_BUFFER_T));
    buffer->data = (char *)malloc(buffer_len);
    if (buffer->data == NULL)
    {
        printf("Malloc buffer error! buffer lens: %zu\n", buffer_len);
        if (buffer != NULL)
        {
            free(buffer);
            buffer = NULL;
        }

        return NULL;
    }
    buffer->size = buffer_len;
    return buffer;
}

void uninit_curl_buffer(CURL_BUFFER_T ** buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    CURL_BUFFER_T * curl_buffer = (CURL_BUFFER_T *) * buffer;
    if (curl_buffer == NULL)
    {
        *buffer = NULL;
        return;
    }

    if (curl_buffer->data != NULL)
    {
        free(curl_buffer->data);
        curl_buffer->data = NULL;
    }

    free(curl_buffer);
    curl_buffer = NULL;
    *buffer = NULL;
}

int32_t curl_get_data(char * url, CURL_BUFFER_T * buffer)
{
    int res = -1;
    CURL * curl = NULL;
    if (buffer == NULL)
    {
        return res;
    }

    curl = curl_easy_init();
    if (curl == NULL) {
        printf ("Curl easy init failed.\n");
        return res;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url); //设置下载地址

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, DEFAULT_CURL_TIMEOUT);//设置超时时间

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置写数据的函数

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);//设置写数据的变量

    res = curl_easy_perform(curl);//执行下载

    if(CURLE_OK != res)
    {
        res = -1;//判断是否下载成功
        goto GET_EXIT;
    }

GET_EXIT:
    if (curl != NULL)
        curl_easy_cleanup(curl);

    return res;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *user_data)
{
    size_t ret;
    CURL_BUFFER_T *buffer = (CURL_BUFFER_T *)user_data;
    int len = size*nmemb;

    if ((buffer->size - buffer->len) > len)
    {
        memcpy(buffer->data + buffer->pos, ptr, len);
        buffer->pos += len;
        buffer->len = len;
    }
    else
    {
        len = buffer->size - buffer->len;
        memcpy(buffer->data + buffer->pos, ptr, len);
        buffer->pos += len;
        buffer->len = len;
        printf("Error: buffer is to small to store the response message\n");
    }

    ret = len;
    // printf("write_buffer buffer->pos=%d, buffer->len=%d, buffer->size:%d, written:%d, user_data:%p\n",
    //     buffer->pos,
    //     buffer->len,
    //     buffer->size,
    //     len,
    //     user_data);

    return ret;
}