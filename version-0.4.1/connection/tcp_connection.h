/**
 * @file tcp_connection.h
 * @author mufiye (1433137434@qq.com)
 * @brief 
 * @version 0.1.1
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * 作为所有具体应用层连接（比如http_connection的父类或是抽象类？还是组成关系？）
 */
#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "../buffer/rw_buffer.h"

class tcp_connection
{
protected:
    int connFd;
    /* 
     * 表示目前这个连接在处理的事件的类型，分为读、写、业务处理
     * 可以考虑改为enum
     */
    int eventType; 

    rw_buffer *input_buffer;
    rw_buffer *output_buffer;
    static std::map<int,tcp_connection*> mp;
    static int m_epollFd;

public:
    tcp_connection(/* args */);
    ~tcp_connection();
    void setConnFd(int connFd);
    int getConnFd();
    int read_data();
    int send_data();
    rw_buffer *get_input_buffer();
    rw_buffer *get_output_buffer();
    void close_connection();

    static void insert_tcp_conn(int fd, tcp_connection* tcp_conn);
    static void remove_tcp_conn(int fd);
    static void remove_all_tcp_conn();
    static tcp_connection* get_tcp_conn(int fd);
    static void set_epollFd(int);

    virtual void process() = 0;
    virtual bool isLinger() = 0;
    virtual void reset() = 0;
};

#endif