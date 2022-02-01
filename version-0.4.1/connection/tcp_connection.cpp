#include "tcp_connection.h"
#include "../log_system/log_system.h"

int tcp_connection::m_epollFd = -1;
std::map<int, tcp_connection *> tcp_connection::mp;

tcp_connection::tcp_connection(/* args */)
{
    LOG_INFO("the tcp conection is built\n");
    this->input_buffer = new rw_buffer(65536);
    this->output_buffer = new rw_buffer(65536);
}

tcp_connection::~tcp_connection()
{
    LOG_INFO("Into tcp_connection::~tcp_connection()\n");
    /* 取消监听 */
    removefd(m_epollFd, connFd);

    /* 将连接从map中移除 */
    tcp_connection::remove_tcp_conn(connFd);

    /* 关闭连接 */
    close_connection();
    connFd = -1;

    /* 清除读写缓冲区 */
    delete this->input_buffer;
    delete this->output_buffer;
    this->input_buffer = nullptr;
    this->output_buffer = nullptr;
}

void tcp_connection::setConnFd(int connFd)
{
    this->connFd = connFd;
}
int tcp_connection::getConnFd()
{
    return this->connFd;
}

int tcp_connection::read_data()
{
    LOG_INFO("pthread: %ld, into to function tcp_connection::read_data()\n", pthread_self());
    return this->input_buffer->buffer_socket_read(this->connFd);
}

int tcp_connection::send_data()
{
    return this->output_buffer->buffer_socket_send(this->connFd);
}

rw_buffer *tcp_connection::get_input_buffer()
{
    return this->input_buffer;
}

rw_buffer *tcp_connection::get_output_buffer()
{
    return this->output_buffer;
}

/* 可能有问题。有待改进？ */
void tcp_connection::close_connection()
{
    close(connFd);
}

void tcp_connection::insert_tcp_conn(int fd, tcp_connection *tcp_conn)
{
    mp.insert(std::pair<int, tcp_connection *>(fd, tcp_conn));
}

void tcp_connection::remove_tcp_conn(int fd)
{
    mp.erase(fd);
}

void tcp_connection::remove_all_tcp_conn(){
    std::map<int, tcp_connection *>::iterator it = mp.begin();
    for(;it != mp.end(); it++)
    {
        mp.erase(it);
    }
}

tcp_connection *tcp_connection::get_tcp_conn(int fd)
{
    return mp[fd];
}

void tcp_connection::set_epollFd(int epollFd)
{
    m_epollFd = epollFd;
}