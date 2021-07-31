#include <stdio.h>
#include "sock.h"


#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFSIZE 1024

typedef struct filedata{
    int type;
    int index;
    char data[BUFSIZE];
}filedata;

static int extractfileinfo(const char *filename)
{
    // 打开文件
    int fd = open(filename,O_RDONLY);

    struct stat state;
    stat(filename,&state);

    return state.st_size;

    close(fd);
}



int main()
{
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    int sockfd = sock_init(PORT,SERVER_IP,&server);

    if(sockfd == -1)
    {
        fprintf(stderr,"sockinit failed!\n");
        return -1;
    }

    
    // 发送的数据包
    char sendbuff[BUFSIZE];
    // 文件名
    char filename[32] = {0};
    while(1)
    {
        printf("请输入文件名:");
        scanf("%s",filename);

        // 过去文件大小
        int filesize = extractfileinfo(filename);
        int type = 1;
        memmove(sendbuff,(void*)&type,sizeof(type));
        memmove(sendbuff+sizeof(type),(void*)&filesize,sizeof(filesize));
        memmove(sendbuff+sizeof(type)+sizeof(filesize),filename,strlen(filename)+1);    // '\0'
        // 发送文件信息
        sendto(sockfd, sendbuff, sizeof(type) + sizeof(filesize)+strlen(filename), 0, (struct sockaddr *)&server, len);
        printf("文件名已发送\n");
        usleep(5);

        filedata fp;
        bzero(&fp,sizeof(fp));
        // 发送文件句柄
        int fd = open(filename,O_RDONLY);
        if(fd == -1){
            fprintf(stderr,"%s open failed",filename);
            return -1;
        }
        // 索引位置
        int index = 0;

        int sendsized = 0;
        while(1)
        {
            fp.type = 2;
            fp.index = index++; 
            size_t size = read(fd,fp.data,BUFSIZE);
            if(size < 0){
                fprintf(stderr,"read failed!\n");
                close(fd);
                return -1;
            }else if(size == 0){
                close(fd);
                break;
            }else{
                memmove(sendbuff,(void*)&(fp.type),sizeof(fp.type));
                memmove(sendbuff+sizeof(type),(void*)&(fp.index),sizeof(fp.index));
                printf("fp.index:%d\n",fp.index);
                memmove(sendbuff+sizeof(fp.type)+sizeof(fp.index),(void *)fp.data,size);    // '\0'
                
                // 发送文件信息
                sendto(sockfd, sendbuff, sizeof(fp.type) + sizeof(fp.index) + size, 0, (struct sockaddr *)&server, len);

                usleep(8);
                sendsized += size;
            }   
        }
        printf("文件传输完毕!filesize = %d,sendsized = %d\n",filesize,sendsized);
    }

    close(sockfd);

    return 0;
}