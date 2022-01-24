#include "../subTry4_rwBuf/rw_buffer.h"

class tcp_connection
{
private:
    int connFd;
    /* 
     * 表示目前这个连接在处理的事件的类型，分为读、写、业务处理
     * 可以考虑改为enum
     */
    int eventType; 

    rw_buffer *input_buffer;
    rw_buffer *output_buffer;

public:
    tcp_connection(/* args */);
    ~tcp_connection();
};

tcp_connection::tcp_connection(/* args */)
{
}

tcp_connection::~tcp_connection()
{
}
