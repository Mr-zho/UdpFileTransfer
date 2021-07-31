#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8888

#define BUFSIZE 1500
#define FILESZIE 64

#define RECVSIZE 1024

int main()
{
    // 建立通信句柄
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);  
    if(sockfd == -1)
    {
        fprintf(stderr,"socket error!");
        return -1;
    }

    struct sockaddr_in server;
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);


    int ret = bind(sockfd,(const sockaddr *)&server,sizeof(server));
    if(ret == -1)
    {
        fprintf(stderr,"bind error!\n");
        return -1;
    }

    char recvbuf[BUFSIZE] = {0};
    char filename[BUFSIZE] = {0};
    while(1)
    {
        memset(recvbuf,0,sizeof(recvbuf));
        int num = recvfrom(sockfd,recvbuf,sizeof(recvbuf),0,0,0);
        if(num < 0)
        {
            fprintf(stderr,"recvfrom error!\n");
            return -1;
        }
        recvbuf[num] = '\0';
        usleep(1);

        
        int type = 0;
        memmove((void*)&type,recvbuf,sizeof(type));
        printf("type:%d\n",type);

        // 地址指针
        static char * ptr = NULL;

        // 文件句柄
        static int fd = 0;

        // 文件大小
        static int filesize = 0;

        // 位置索引
        int index = 0;

        // 已经接收到的文件大小
        static int recvedsize = 0;

        char data[RECVSIZE] = {0};
        switch (type)
        {
        case 1: // 文件信息
        {
            #if 0
            memmove(&filesize,recvbuf+sizeof(type),sizeof(filesize));
            memmove(filename,recvbuf+sizeof(filesize)+sizeof(type),sizeof(filename));

            printf("接收到的filename:%s,filesize:%d\n",filename,filesize);

            ptr = (char *)calloc(1,sizeof(char)*filesize);
            if(ptr == NULL)
            {
                fprintf(stderr,"calloc failed!\n");
            }

            fd = open(filename,O_WRONLY|O_CREAT);
            if(fd == -1)
            {
                fprintf(stderr,"%s open failed",filename);
                return -1;
            }
            #endif

            memmove((void*)&filesize,recvbuf+sizeof(type),sizeof(filesize));
            memmove((void*)filename,recvbuf+sizeof(type)+sizeof(filesize),num - sizeof(type) - sizeof(filesize));
            
            printf("客户端发送过来的文件信息是:文件名称:%s,文件大小:%d\n",filename,filesize);
            // 准备好一块空地
            ptr = (char *)calloc(1,sizeof(char) * filesize);
            if(ptr == NULL)
            {
                fprintf(stderr,"pRecvData calloc error");
                return -1;
            }

            fd = open(filename,O_WRONLY|O_CREAT);
            if(fd == -1)
            {
                fprintf(stderr,"%s open error",filename);
                return -1;
            }

        }
            break;
        case 2: // 文件内容
        {
            #if 0
            memset(data,0,sizeof(data));
            memmove(&index,recvbuf+sizeof(type),sizeof(index));
            printf("index:%d\n",index);
            memmove(data,recvbuf+sizeof(type)+sizeof(index),num - sizeof(type) - sizeof(index));

            memmove(ptr+(index)*(num - sizeof(type) - sizeof(index)),data,num - sizeof(type) - sizeof(index));

            recvedsize += num - sizeof(type) - sizeof(index);
            #endif

            memmove((void*)&index,recvbuf+sizeof(type),sizeof(index));
            memmove((void*)data,recvbuf+sizeof(type)+sizeof(index),num - sizeof(type) - sizeof(index));
            
            printf("index:%d\n",index);
            memmove(ptr + (index)*(num - sizeof(index) - sizeof(type)),data,num - sizeof(int)*2);
            
            recvedsize += num - sizeof(type) - sizeof(index);
        }
            break;
        default:   
            fprintf(stderr,"type error!");
            break;
        }
        printf("recvedsize:%d,filesize:%d\n",recvedsize,filesize);
        if(recvedsize == filesize && recvedsize != 0)
        {
            printf("文件接收成功!\n");

            write(fd,ptr,filesize);

            close(fd);

            recvedsize = 0;
        }
        
    }
    

    return 0;
}