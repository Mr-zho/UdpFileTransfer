#include "sock.h"


/**
 * 获取JSON值json的类型
 * @param port json值
 * @param ip json值
 * @return 成功返回通信句柄，失败返回-1
 */
int sock_init(int port,const char * ip,struct sockaddr_in *server)
{
    int sockfd;     //file discription
    // 创建套接字句柄
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket error!\n");
        return -1;
    }

    
    
    bzero(server, sizeof(server));
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    server->sin_addr.s_addr = inet_addr(ip);


    return sockfd;
}