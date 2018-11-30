//socket udp 客户端
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>

#include "handle.h"

#define DEFAULT_CURL_LENGTH 20480

#define MAX_BUFF_SIZE 1024
#define SERVER_PORT 1900
#define SERVER_IP "239.255.255.250"

#define DEFAULT_CURL_TIMEOUT 2000
#define DEFAULT_MAX_LENGTH 209600

#define LOCATION "LOCATION:"

#define IF_NOT_NULL_FREE(str) do { \
    if (str != NULL) { \
        free(str); \
        str = NULL; \
    } \
} while(0)

// #define DISCOVER "M-SEARCH * HTTP/1.1 \
// HOST: 239.255.255.250:1900 \
// MAN: \"ssdp:discover\" \
// MX: seconds to delay response \
// ST: search target"

static void * handle_recv_packet(void * data);
static char * get_location(char * data);

int main(int argc, char * argv[])
{
    int ret = -1;
    //创建socket对象
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    //创建网络通信对象
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    char buf[MAX_BUFF_SIZE];
    strcat(buf, "M-SEARCH * HTTP/1.1\r\n");
    strcat(buf, "HOST: 239.255.255.250:1900\r\n");
    strcat(buf, "MAN: \"ssdp:discover\"\r\n");
    strcat(buf, "MX: 5\r\n");
    strcat(buf, "ST: urn:schemas-upnp-org:device:MediaRenderer:1\r\n");
    strcat(buf, "\r\n\r\n");
    printf("send buf:\n%s\n", buf);

    ret = sendto(sockfd, buf, strlen(buf) + 1, 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 0)
    {
        printf("send error: %s\n", strerror(errno));
        return -1;
    }

    memset(buf, 0, MAX_BUFF_SIZE);
    while((ret = recv(sockfd, buf, MAX_BUFF_SIZE - 1, 0)) > 0)
    {
        printf("################ recv form server: #################\n%s\n", buf);
        pthread_t handle_id;
        char * packet = strdup(buf);
        if (pthread_create(&handle_id, NULL, handle_recv_packet, (void *) packet))
        {
            pthread_detach(handle_id);
        }

        memset(buf, 0, MAX_BUFF_SIZE);
    }

    if (ret < 0)
    {
        printf("recv error: %s\n", strerror(errno));
    }
    else if (ret == 0)
    {
        printf("server is disconnected!\n");
    }

    close(sockfd);
}

void * handle_recv_packet(void * data)
{
    CURL_BUFFER_T * res_data = NULL;
    char * packet = (char *)data;
    char * url = get_location(packet);
    if (url == NULL)
    {
        printf("didnot find %s\n", LOCATION);
        goto THREAD_EXIT;
    }

    printf("url: %s\n", url);
    res_data = init_curl_buffer(DEFAULT_CURL_LENGTH);
    if (res_data == NULL)
    {
        goto THREAD_EXIT;
    }

    curl_get_data(url, res_data);
    printf("res_data: \n%s\n", res_data->data);

THREAD_EXIT:
    uninit_curl_buffer(&res_data);
    IF_NOT_NULL_FREE(url);
    IF_NOT_NULL_FREE(packet);

    return NULL;
}

char * get_location(char * data)
{
    int i = 0;
    char * location = NULL;
    if (data == NULL)
    {
        return NULL;
    }

    location = strstr(data, LOCATION);
    if (location == NULL)
    {
        return NULL;
    }

    location += strlen(LOCATION);
    while (*(location) == ' ' || *location == '\t')
        location++;

    for (i = 0; *(location + i) != '\n' && *(location + i) != '\r'; i++);

    char * url = (char *)malloc(i + 1);
    if (url == NULL)
    {
        printf("Malloc error!\n");
        return NULL;
    }

    memset(url, 0, i + 1);
    strncpy(url, location, i);

    return url;
}

/*
<?xml version="1.0"?>
<root
    xmlns="urn:schemas-upnp-org:device-1-0">
    <specVersion>
        <major>1</major>
        <minor>0</minor>
    </specVersion>
    <device>
        <deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>
        <presentationURL>/</presentationURL>
        <friendlyName>客厅的小米盒子</friendlyName>
        <manufacturer>Xiaomi</manufacturer>
        <manufacturerURL>http://www.xiaomi.com/</manufacturerURL>
        <modelDescription>Xiaomi MediaRenderer</modelDescription>
        <modelName>Xiaomi MediaRenderer</modelName>
        <modelURL>http://www.xiaomi.com/hezi</modelURL>
        <UPC>000000000000</UPC>
        <UDN>uuid:F7CA5454-3F48-4390-8009-4c3d4ee64717</UDN>
        <UID>-7504971316544787133</UID>
        <serviceList>
            <service>
                <serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>
                <serviceId>urn:upnp-org:serviceId:AVTransport</serviceId>
                <SCPDURL>/dlna/Render/AVTransport_scpd.xml</SCPDURL>
                <controlURL>_urn:schemas-upnp-org:service:AVTransport_control</controlURL>
                <eventSubURL>_urn:schemas-upnp-org:service:AVTransport_event</eventSubURL>
            </service>
            <service>
                <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>
                <serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>
                <SCPDURL>/dlna/Render/ConnectionManager_scpd.xml</SCPDURL>
                <controlURL>_urn:schemas-upnp-org:service:ConnectionManager_control</controlURL>
                <eventSubURL>_urn:schemas-upnp-org:service:ConnectionManager_event</eventSubURL>
            </service>
            <service>
                <serviceType>urn:schemas-upnp-org:service:RenderingControl:1</serviceType>
                <serviceId>urn:upnp-org:serviceId:RenderingControl</serviceId>
                <SCPDURL>/dlna/Render/RenderingControl_scpd.xml</SCPDURL>
                <controlURL>_urn:schemas-upnp-org:service:RenderingControl_control</controlURL>
                <eventSubURL>_urn:schemas-upnp-org:service:RenderingControl_event</eventSubURL>
            </service>
            <service>
                <serviceType>urn:mi-com:service:RController:1</serviceType>
                <serviceId>urn:upnp-org:serviceId:RController</serviceId>
                <SCPDURL>/dlna/Render/RControl_scpd.xml</SCPDURL>
                <controlURL>_urn:schemas-upnp-org:service:RenderingControl_control</controlURL>
                <eventSubURL>_urn:schemas-upnp-org:service:RenderingControl_event</eventSubURL>
            </service>
        </serviceList>
        <av:X_RController_DeviceInfo
            xmlns:av="urn:mi-com:av">
            <av:X_RController_Version>1.0</av:X_RController_Version>
            <av:X_RController_ServiceList>
                <av:X_RController_Service>
                    <av:X_RController_ServiceType>controller</av:X_RController_ServiceType>
                    <av:X_RController_ActionList_URL>http://192.168.50.210:6095/</av:X_RController_ActionList_URL>
                </av:X_RController_Service>
                <av:X_RController_Service>
                    <av:X_RController_ServiceType>data</av:X_RController_ServiceType>
                    <av:X_RController_ActionList_URL>http://api.tv.duokanbox.com/bolt/3party/</av:X_RController_ActionList_URL>
                </av:X_RController_Service>
            </av:X_RController_ServiceList>
        </av:X_RController_DeviceInfo>
    </device>
    <URLBase>http://192.168.50.210:49152/</URLBase>
</root>
*/






