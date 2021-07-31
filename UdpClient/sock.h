/*
 *  通信
 *
 */
#ifndef __SOCK_H__
#define __SOCK_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>


// 通信端口
#define PORT 8888

/**
 * 获取JSON值json的类型
 * @param port json值
 * @param ip json值
 * @return 成功返回通信句柄，失败返回-1
 */
int sock_init(int port,const char * ip,struct sockaddr_in *server);

#endif // SOCK_H__