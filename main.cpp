/*
目前暂时无用
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int BACKLOG = 10000; //10K，表示已连接的队列的大小
void runSubReactor()
{
    int eFd;
}
void doWork(void *data)
{
    pthread_detach(pthread_self());
    int connFd = (int)data;
    int byteNum;
    char buf[1024];
    memset(&buf,0,sizeof(buf));
    sprintf(buf, "hello,client,my tid is %d\n",pthread_self());
    if((byteNum = send(connFd,buf,strlen(buf),0)) != strlen(buf)){
        perror("some wrong when send()\n");
    }
    close(connFd);
}
int main()
{
    int lisFd;
    int connFd;
    lisFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in *serverAddr, *clientAddr;

    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = INADDR_ANY;
    serverAddr->sin_port = htons(8080);

    if (bind(lisFd, serverAddr, strlen(*serverAddr)))
    {
        perror("some wrong when bind()\n");
    }
    if (listen(lisFd, BACKLOG))
    {
        perror("some wrong when listen()\n");
    }
    //优化点：accept应该使用非阻塞I/O
    while (1)
    {
        if ((connFd = accept(lisFd, (sockaddr *)clientAddr, strlen(*clientAddr))))
        {
            /*
             *优化点：需要细化的错误处理
            */
            perror("some wrong when accept()\n");
        }
        pthread_t tid;
        pthread_create(&tid, NULL, doWork, (void *)connFd);
    }
    close(lisFd);
    // //假设有四条sub-reactor线程
    // for (int i = 0; i < 4; i++)
    // {
    // }
}