#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/epoll.h>
#include <cstdlib>
#include <ctime>
#include "http_response.h"

const int BACKLOG = 10000; //10K，表示已连接的队列的大小
int lisFd;  //监听端口

void handleCC(int signo)
{
    if (lisFd >= 0)
    {
        close(lisFd);
        printf("close listen socket\n");
    }
    printf("process exits by ctrl+c\n");
    exit(0);
}

int main()
{
    signal(SIGINT, handleCC);
    int connFd; //连接端口
    lisFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr, clientAddr;

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8080);
    socklen_t servAddrSize = sizeof(serverAddr);
    socklen_t clientLen;

    /*
    *允许重用地址，解决地址被占用问题
    *但是只能使TIME_WAIT状态的连接被重用，LISTEN状态的连接无法重用
    */
    int on = 1;
    if (setsockopt(lisFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("some wrong with setsockopt()");
    }

    if (bind(lisFd, (sockaddr *)&serverAddr, servAddrSize))
    {
        perror("some wrong when bind()");
    }
    if (listen(lisFd, BACKLOG))
    {
        perror("some wrong when listen()");
    }

    char buf[1024];
    int byteNum;
    while (1)
    {
        connFd = accept(lisFd, (sockaddr *)&clientAddr, &clientLen);
        memset(&buf, 0, sizeof(buf));
        printf("the connection is established\n");
        printf("the new connection socket is: %d\n",connFd);
        /**
         * 读取、处理和发送http数据
         */
        http_response http_re;
        char *buffer = http_re.encode_response();
        send(connFd,buffer,strlen(buffer),0);
        //sleep(10);
        close(connFd);
        printf("the connection is closed\n");
    }
}