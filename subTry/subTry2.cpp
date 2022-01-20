/*
*author: mufiye
*completed date: 2022.1.19
*email: 1433137434@qq.com
*----------------------------------------------------
*多线程TCP服务器程序
*主线程负责accept
*子线程负责IO和逻辑处理
*Q1：套接字从主线程传递到子线程总是出错
*A1：一开始使用vector有问题，后来使用数组就没事了
*Q2：总是出现地址已经被使用的情况
*A2：setsockopt()使TIME_WAIT状态的连接能够被重用，但是LISTEN状态的连接无法被重用
*Q3：由问题2引出的问题3，如何处理时刻处于监听状态的端口
*A3：ctrl+c/z引发的进程终止应当对应于某个信号量，编写对应的信号量处理函数！
*    只能编写ctrl+c的信号量处理函数，因为ctrl+z引发的信号量无法被程序捕获。
*----------------------------------------------------

*----------------------------------------------------
*使用非阻塞IO+select/poll/epoll实现accept
*(为了将监听套接字和普通套接字可以一起监听？)
*1. 关于将listen套接字设为非阻塞的
*   Q1：为什么accept要设为非阻塞的？
    A1：为了在多路复用accept的时候不会阻塞在一个错误的情况上面
*2. 使用select实现(实现)
*3. 使用poll实现(有bug)
*4. 使用epoll实现(有bug)
*----------------------------------------------------

*----------------------------------------------------
* 一些最佳实践：
* 1. perror里面的字符不要接换行符，因为在你自己想输出的字符后有系统的提示信息
*----------------------------------------------------
*/
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
#include <vector>

const int INIT_SIZE = 128;
const int MAXEVENTS = 128; //epoll监听的最多事件数量
const int BACKLOG = 10000; //10K，表示已连接的队列的大小
int lisFd;                 //监听端口

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
void *doWork(void *data)
{
    pthread_detach(pthread_self());
    //sleep(5);
    sleep(rand() % 5);
    int *connFd = (int *)data;
    //printf("the connFd is %d\n", *connFd);
    /*
    *只对新建立的连接有效，对已建立的连接无效
    */
    // int on = 1;
    // setsockopt(*connFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    int byteNum;
    char buf[1024];
    memset(&buf, 0, sizeof(buf));
    sprintf(buf, "hello,client,my tid is %ld", pthread_self());
    if ((byteNum = send(*connFd, buf, strlen(buf), 0)) <= 0)
    {
        perror("some wrong when send()");
    }
    else
    {
        printf("My pthread id is: %ld, and I send successfully.\n", pthread_self());
    }
    close(*connFd);
    return nullptr;
}
int main()
{
    //关联信号量处理函数
    signal(SIGINT, handleCC);
    srand((int)time(0));
    int connFd;
    lisFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr, clientAddr;

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(12350);
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

    int i = 0;
    int args[100];
    /*
    *对listen套接字使用非阻塞IO 
    */
    //fcntl(lisFd, F_SETFL, O_NONBLOCK);
    while (1)
    {
        connFd = accept(lisFd, (sockaddr *)&clientAddr, &clientLen);
        if (connFd < 0)
        {
            perror("accept failed");
        }
        else
        {
            printf("accept success, connection file descriptor is %d\n", connFd);
            pthread_t tid;
            args[i] = connFd;
            pthread_create(&tid, NULL, doWork, (void *)&args[i]);
            i++;
        }
    }
    //------------------------------------------------------------
    /*
    *select()模块
    */
    // fd_set readSet, writeSet, expSet;
    // while (1)
    // {
    //     FD_ZERO(&readSet);
    //     FD_ZERO(&writeSet);
    //     FD_ZERO(&expSet);
    //     FD_SET(lisFd, &readSet);
    //     if(select(lisFd + 1, &readSet, &writeSet, &expSet, NULL) < 0){
    //         perror("some wrong when select()");
    //     }
    //     if(FD_ISSET(lisFd,&readSet)){
    //         connFd = accept(lisFd, (sockaddr *)&clientAddr, &clientLen);
    //         if(connFd < 0){
    //             perror("accept failed");
    //         }
    //         else{
    //             printf("accept success, connection file descriptor is %d\n",connFd);
    //         }
    //         // //select有1024的监听限制
    //         // else if(connFd > 1024){

    //         // }
    //     }
    //     pthread_t tid;
    //     args[i] = connFd;
    //     pthread_create(&tid, NULL, doWork, (void *)&args[i]);
    //     i++;
    // }

    //-----------------------------------------------------------
    /*
    *poll模块，有错误
    */

    // struct pollfd event_set[INIT_SIZE];
    // event_set[0].fd = lisFd;
    // event_set[0].events = POLLRDNORM;
    // int ready_number;
    // for (i = 1; i < INIT_SIZE; i++)
    // {
    //     event_set[i].fd = -1;
    // }
    // while (1)
    // {
    //     if ((ready_number = poll(event_set, INIT_SIZE, -1)) < 0)
    //     {
    //         /*perror函数也可以封装
    //         *1. 封装成包含errno信息的
    //         *2. 同时可以指定输出的文件描述符(某个文件的话就是日志，或者输出到控制台)
    //         */
    //         perror("some wrong when poll()");
    //     }
    //     if (event_set[0].revents & POLLRDNORM)
    //     {
    //         connFd = accept(lisFd, (sockaddr *)&clientAddr, &clientLen);
    //         if (connFd < 0)
    //         {
    //             perror("accept failed");
    //         }
    //         else
    //         {
    //             printf("accept success, connection file descriptor is %d\n", connFd);
    //         }
    //     }

    //     pthread_t tid;
    //     args[i] = connFd;
    //     pthread_create(&tid, NULL, doWork, (void *)&args[i]);
    //     i++;
    // }

    //---------------------------------------------------------
    /*
    *epoll模块，有错误
    */
    // int efd = epoll_create1(0);
    // if (efd == -1)
    // {
    //     perror("some wrong when create epoll file descriptor");
    // }

    // struct epoll_event event;
    // struct epoll_event *events;

    // event.data.fd = lisFd;
    // event.events = EPOLLIN | EPOLLET;

    // if (epoll_ctl(efd, EPOLL_CTL_ADD, lisFd, &event) == -1)
    // {
    //     perror("epoll_ctl add listen fd failed");
    // }

    // while (1)
    // {
    //     int n = epoll_wait(efd, events, MAXEVENTS, -1);
    //     printf("epoll_wait wakeup\n");
    //     for (int i = 0; i < n; i++)
    //     {
    //         if ((events[i].events & EPOLLERR) ||
    //             (events[i].events & EPOLLHUP) ||
    //             (!(events[i].events & EPOLLIN)))
    //         {
    //             perror("epoll error\n");
    //             close(events[i].data.fd);
    //             exit(0);
    //         }
    //         else if (lisFd == events[i].data.fd)
    //         {
    //             connFd = accept(lisFd, (sockaddr *)&clientAddr, &clientLen);
    //             if (connFd < 0)
    //             {
    //                 perror("accept failed");
    //                 break;
    //             }
    //             else
    //             {
    //                 printf("accept success, connection file descriptor is %d\n", connFd);
    //             }
    //             args[i] = connFd;
    //             pthread_t tid;
    //             pthread_create(&tid, NULL, doWork, (void *)&args[i]);
    //             i++;
    //         }
    //     }
    // }
    close(lisFd);
}