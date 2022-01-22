/*
*支持多线程的tcp客户端程序
*author: mufiye
*completed date: 2022.1.19
*email: 1433137434@qq.com
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>

void *doClient(void *data)
{
    //printf("my pthread id is: %d\n",pthread_self());
    //pthread_detach(pthread_self());
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(10000);
    socklen_t servAddrSize = sizeof(serverAddr);
    // sockaddr_in *clientAddr;
    // clientAddr->sin_family = AF_INET;
    // clientAddr->sin_addr.s_addr = INADDR_ANY;
    // clientAddr->sin_port = htons(10000 + i);
    /*
    *让客户端随机睡一段时间
    */
    //sleep(rand() % 5);
    //如果服务器端是非阻塞IO，这里会返回EINPROGRESS错误
    while (1)
    {
        if (connect(clientFd, (sockaddr *)&serverAddr, servAddrSize) < 0 && errno == EINPROGRESS)
        {
            printf("encounter EINPROGRESS, but it's all right\n");
            continue;
        }
        /*
        *写操作验证模块   
        */
        int byteNum;
        char buf[1024];
        memset(&buf, 0, sizeof(buf));
        sprintf(buf, "hello,server,my tid is %ld", pthread_self());
        if ((byteNum = send(clientFd, buf, strlen(buf), 0)) <= 0)
        {
            perror("some wrong when send()");
        }
        else
        {
            printf("My pthread id is: %ld, and I send successfully.\n", pthread_self());
        }
        /*
        *读操作验证模块
        */
        memset(&buf, 0, sizeof(buf));
        if ((byteNum = recv(clientFd, buf, sizeof(buf), 0)) <= 0)
        {
            perror("some wrong when read()");
        }
        else
        {
            printf("My pthread id is: %ld, and I receive: %s.\n", pthread_self(), buf);
        }
        break;
    }
    close(clientFd);
    return nullptr;
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("you should input the number of clients\n");
        exit(1);
    }
    int clientNum = atoi(argv[1]);
    srand((int)time(0));
    pthread_t tids[clientNum];
    for (int i = 0; i < clientNum; i++)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, doClient, NULL);
        tids[i] = tid;
    }
    for (int i = 0; i < clientNum; i++)
    {
        pthread_join(tids[i], NULL);
    }
    return 0;
}