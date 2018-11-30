//socket udp 客户端
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>

#include <string.h>
#include <errno.h>

//#include "cJSON.h"

#define MAX_BUFF_SIZE 1024

#define SERVER_PORT 1900
#define SERVER_IP "239.255.255.250"

// #define DISCOVER "M-SEARCH * HTTP/1.1 \
// HOST: 239.255.255.250:1900 \
// MAN: \"ssdp:discover\" \
// MX: seconds to delay response \
// ST: search target"

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












