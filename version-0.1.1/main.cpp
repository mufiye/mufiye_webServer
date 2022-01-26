/**
 * @file main.cpp
 * @author mufiye (1433137434@qq.com)
 * @brief 
 * @version 0.1.1
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * 主线程文件
 */
#include "./acceptor/acceptor.h"
//#include "./locker/locker.h"
#include "./buffer/rw_buffer.h"
#include "./threadPool/threadPool.h"
#include "./connection/simple_connection.h"

const int MAX_EVENT_NUMBER = 4096;
extern void addfd(int, int, bool);
extern void removefd(int, int);
/* 使用connFd找到一个tcp连接 */

int main(int argc, char *argv[])
{
    int port; //监听的端口号
    int listenFd; //监听套接字
    int epollFd;
    epoll_event events[MAX_EVENT_NUMBER];
    /* 创建线程池 */
    threadPool<tcp_connection> *pool = new threadPool<tcp_connection>();
    bool server_stop = false;
    if (argc <= 1)
    {
        printf("usage: input port_number\n");
        return 1;
    }

    port = atoi(argv[1]);
    acceptor ap(port);
    listenFd = ap.getListenFd();
    assert(listenFd >= 0);
    epollFd = epoll_create1(0);
    assert(epollFd != -1);
    tcp_connection::set_epollFd(epollFd);
    addfd(epollFd,listenFd,false);

    while(!server_stop){
        int number = epoll_wait(epollFd,events,MAX_EVENT_NUMBER,-1);
        if(number < 0){
            break;
        }
        //printf("the number is %d\n",number);
        for(int i=0; i<number; i++){
            int sockFd = events[i].data.fd;
            if(sockFd == listenFd){
                /* 做accept操作 */
                int connFd = ap.getConnFd();
                //printf("the connFd is %d\n",connFd);
                /* 优化点：获取客户端地址信息？*/
                assert(connFd >= 0);
                tcp_connection *simple_conn = new simple_connection();
                simple_conn->setConnFd(connFd);
                tcp_connection::insert_tcp_conn(connFd,simple_conn);
                addfd(epollFd,connFd,true);
                //process()如何书写
                //读写描述符和事件何时加入epoll Fd
            }else if(events[i].events & EPOLLIN){
                int connFd = events[i].data.fd;
                tcp_connection* tcp_conn = tcp_connection::get_tcp_conn(connFd); 
                /* 读取数据 */
                if(tcp_conn->read_data()){
                    pool->append(tcp_conn);
                }
            }else if(events[i].events & EPOLLOUT){
                int connFd = events[i].data.fd;
                tcp_connection* tcp_conn = tcp_connection::get_tcp_conn(connFd); 
                /* 发送数据 */
                if(tcp_conn->send_data()){
                    printf("I have send all data\n");
                    removefd(epollFd,connFd);
                    close(connFd);
                }
            }
        }
    }
    close(listenFd);
    close(epollFd);
    delete pool;
    return 0;
}