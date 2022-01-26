#include "simple_connection.h"

extern void modfd(int, int, int);

simple_connection::simple_connection(/* args */):tcp_connection()
{

}

simple_connection::~simple_connection()
{

}

void simple_connection::process(){
    printf("into the simple connection's process function\n");
    /* 读取输入缓冲区中的数据并显示 */
    printf("I read: %s \n", this->input_buffer->get_buffer_data()+this->input_buffer->get_buffer_read_index());
    /* 改变输出缓冲区中的数据 */
    char buf[1024];
    snprintf(buf,1024,"hello, I'm server, my thread id is %ld\n",pthread_self());
    this->output_buffer->buffer_append_string(buf);
    modfd(m_epollFd, this->connFd, EPOLLOUT);
    /* 将套接字加入epollFd,发送数据 */
    //addfd(m_epollFd,this->connFd,true);
}