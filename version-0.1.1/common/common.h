/**
 * @file common.h
 * @author mufiye (1433137434@qq.com)
 * @brief 
 * @version 0.1.1
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * 通用头文件，包括一些通用的头文件和封装的方法
 */
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <exception>
#include <list>
#include <stdio.h>
#include <poll.h>
#include <assert.h>
#include <map>

//对文件描述符设置非阻塞
int setnonblocking(int fd);

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void addfd(int epollfd, int fd, bool one_shot);

//从内核时间表删除描述符
void removefd(int epollfd, int fd);

//将事件重置为EPOLLONESHOT
void modfd(int epollfd, int fd, int ev);

#endif
