/**
*author: mufiye
*estimated time of completion: 2022.1.20
*email: 1433137434@qq.com
*---------------------------------------------------
*两个线程
*一个线程负责accept()
*一个线程负责IO多路复用读写套接字
*---------------------------------------------------
*poll模块
*不知道为什么有时会在accept处出现错误，错误信息为：Invalid argument
*---------------------------------------------------
*epoll模块
*
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

const int INIT_SIZE = 128;
const int MAXEVENTS = 128;
const int BACKLOG = 10000; //10K，表示已连接的队列的大小
int lisFd;                 //监听端口

/*
*poll block
*/
// struct pollfd event_set[INIT_SIZE];

/**
 * epoll block
 */
int efd;
struct epoll_event event;
struct epoll_event *events;

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

void *doIOWork(void *data)
{
    int byteNum;
    char buf[1024];
    //memset(&buf, 0, sizeof(buf));
    //sleep(10);

    /**
     * poll模块
     */
    // int ready_number;
    // while (1)
    // {
    //     if ((ready_number = poll(event_set, INIT_SIZE, 5000)) < 0)
    //     {
    //         perror("some wrong with poll");
    //         continue;
    //     }
    //     else if (ready_number == 0)
    //     {
    //         perror("poll timeout");
    //         continue;
    //     }
    //     for (int i = 0; i < INIT_SIZE && ready_number > 0; i++)
    //     {
    //         if (event_set[i].revents & POLLIN)
    //         {
    //             printf("in\n");
    //             ready_number--;
    //             /*
    //             *做读取操作
    //             */
    //             memset(&buf, 0, sizeof(buf));
    //             if ((byteNum = recv(event_set[i].fd, buf, sizeof(buf), 0)) <= 0)
    //             {
    //                 perror("some wrong with recv()");
    //             }
    //             else
    //             {
    //                 printf("I receive: %s\n", buf);
    //                 memset(&buf, 0, sizeof(buf));
    //                 sprintf(buf, "hello,client,my tid is %ld", pthread_self());
    //                 if (send(event_set[i].fd, buf, strlen(buf), 0) <= 0)
    //                 {
    //                     perror("some wrong when send()");
    //                 }
    //                 close(event_set[i].fd);
    //                 event_set[i].fd = -1;
    //             }
    //         }
    //     }
    // }

    /**
     * epoll模块
     */
    events = (epoll_event*)malloc(sizeof(epoll_event) * MAXEVENTS);
    while (1)
    {
        int n = epoll_wait(efd, events, MAXEVENTS, 5000);
        if (n == 0)
        {
            //表示超时
            perror("epoll timeout");
            continue;
        }
        else if (n < 0)
        {
            perror("some wrong when epoll");
            continue;
        }
        for (int i = 0; i < n; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                memset(&buf, 0, sizeof(buf));
                if ((byteNum = recv(events[i].data.fd, buf, sizeof(buf), 0)) <= 0)
                {
                    perror("some wrong with recv()");
                }
                else
                {
                    printf("I receive: %s\n", buf);
                    memset(&buf, 0, sizeof(buf));
                    sprintf(buf, "hello,client,my tid is %ld", pthread_self());
                    if (send(events[i].data.fd, buf, strlen(buf), 0) <= 0)
                    {
                        perror("some wrong when send()");
                    }
                    epoll_ctl(efd, EPOLL_CTL_DEL, events[i].data.fd, &event);
                    close(events[i].data.fd);
                }
            }
        }
    }
    free(events);
    return nullptr;
}

int main()
{
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
    /**
     *poll block
     */
    //初始化poll fd
    // for (int i = 0; i < INIT_SIZE; i++)
    // {
    //     event_set[i].fd = -1;
    // }

    /**
     * epoll block
     */
    efd = epoll_create1(0);

    //创建处理IO的线程
    pthread_t tid;
    pthread_create(&tid, NULL, doIOWork, NULL);

    /*
    *对listen套接字使用非阻塞IO 
    */
    //fcntl(lisFd, F_SETFL, O_NONBLOCK);

    while (1)
    {
        /**
         * poll模块
         */
        // connFd = accept(lisFd, (sockaddr *)&clientAddr, &clientLen);
        // if (connFd < 0)
        // {
        //     perror("accept failed");
        // }
        // else
        // {
        //     printf("accept success, connection file descriptor is %d\n", connFd);
        //     int i = 0;
        //     for (; i < INIT_SIZE; i++)
        //     {
        //         if (event_set[i].fd == -1)
        //         {
        //             event_set[i].fd = connFd;
        //             event_set[i].events = POLLIN;
        //             break;
        //         }
        //     }
        //     if (i == INIT_SIZE)
        //     {
        //         printf("no remain poll set space\n");
        //         close(connFd);
        //         continue;
        //     }
        // }

        /**
         * epoll模块
         */
        connFd = accept(lisFd, (sockaddr *)&clientAddr, &clientLen);
        if (connFd < 0)
        {
            perror("accept failed");
        }
        else
        {
            printf("accept success, connection file descriptor is %d\n", connFd);
            event.data.fd = connFd;
            event.events = EPOLLIN | EPOLLET;
            if (epoll_ctl(efd, EPOLL_CTL_ADD, connFd, &event) == -1)
            {
                perror("epoll_ctl add listen fd failed");
                close(connFd);
                continue;
            }
        }
    }
    return 0;
}