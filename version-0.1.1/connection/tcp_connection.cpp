#include "tcp_connection.h"

int tcp_connection::m_epollFd = -1;
std::map<int,tcp_connection*> tcp_connection::mp;

tcp_connection::tcp_connection(/* args */)
{
    this->input_buffer = new rw_buffer();
    this->output_buffer = new rw_buffer();
}

tcp_connection::~tcp_connection()
{
    delete this->input_buffer;
    delete this->output_buffer;
    this->input_buffer = nullptr;
    this->output_buffer = nullptr;
    close(this->connFd);
    mp.erase(this->connFd);
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

void tcp_connection::insert_tcp_conn(int fd, tcp_connection* tcp_conn){
    mp.insert(std::pair<int,tcp_connection*>(fd,tcp_conn));
}

tcp_connection* tcp_connection::get_tcp_conn(int fd){
    return mp[fd];
}

void tcp_connection::set_epollFd(int epollFd){
    m_epollFd = epollFd;
}