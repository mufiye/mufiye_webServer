#include "tcp_connection.h"


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
}

rw_buffer* tcp_connection::get_input_buffer(){
    return this->input_buffer;
}

rw_buffer* tcp_connection::get_output_buffer(){
    return this->output_buffer;
}