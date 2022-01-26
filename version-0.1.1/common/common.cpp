/* 将函数声明和定义都写在头文件里，会导致多次重复定义 */
#include "common.h"

extern int setnonblocking(int fd);
extern void addfd(int epollfd, int fd, bool one_shot);
extern void removefd(int epollfd, int fd);
extern void modfd(int epollfd, int fd, int ev);

//对文件描述符设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void addfd(int epollfd, int fd, bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;

    // #ifdef connfdET
    //     event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    // #endif

    // #ifdef connfdLT
    //     event.events = EPOLLIN | EPOLLRDHUP;
    // #endif

    // #ifdef listenfdET
    //     event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    // #endif

    // #ifdef listenfdLT
    //     event.events = EPOLLIN | EPOLLRDHUP;
    // #endif
    event.events = EPOLLIN | EPOLLET;
    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//从内核时间表删除描述符
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

//将事件重置为EPOLLONESHOT
void modfd(int epollfd, int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;

    // #ifdef connfdET
    //     event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    // #endif

    // #ifdef connfdLT
    //     event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
    // #endif
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}