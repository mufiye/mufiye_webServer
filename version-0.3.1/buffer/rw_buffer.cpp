#include "rw_buffer.h"

const char *CRLF = "\r\n";
//const int INIT_BUFFER_SIZE = 65536;

rw_buffer::rw_buffer(int size)
{
    this->data = (char *)malloc(size);
    this->init_buffer_size = size;
    this->total_size = size;
    this->readIndex = 0;
    this->writeIndex = 0;
    this->checked_idx = 0;
}

rw_buffer::~rw_buffer()
{
    free(this->data);
}

/* 缓冲区重置 */
void rw_buffer::reset(){
    memset(this->data,0,sizeof this->data);
    this->writeIndex = this->readIndex = this->checked_idx = 0;;
}

/* 为了读取数据，应该有封装性更好的方法 */
char *rw_buffer::get_buffer_data()
{
    return this->data;
}

int rw_buffer::get_buffer_read_index()
{
    return this->readIndex;
}
int rw_buffer::get_buffer_write_index()
{
    return this->writeIndex;
}

/* 获取可写的空间数量 */
int rw_buffer::get_buffer_writeable_size()
{
    return this->total_size - this->writeIndex;
}

/* 获取可读的空间数量 */
int rw_buffer::get_buffer_readable_size()
{
    return this->total_size - this->readIndex;
}

/* 获取空闲的空间数量 */
int rw_buffer::get_buffer_front_spare_size()
{
    return this->readIndex;
}

/* 获取有内容的空间大小 */
int rw_buffer::get_buffer_content_size()
{
    return this->writeIndex - this->readIndex;
}

void rw_buffer::make_room(int size)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function make_room()\n",pthread_self());
    fflush(stdout);
#endif
    if (get_buffer_writeable_size() >= size)
    {
        return;
    }
    if (get_buffer_front_spare_size() + get_buffer_writeable_size() >= size)
    {
        int readable = get_buffer_readable_size();
        int i;
        for (i = 0; i < readable; i++)
        {
            memcpy(this->data + i, this->data + this->readIndex + i, 1);
        }
        this->readIndex = 0;
        this->writeIndex = readable;
    }
    else
    {
        //扩大缓冲区
        void *tmp = realloc(this->data, this->total_size + size);
        if (tmp == NULL)
        {
            return;
        }
        this->data = (char *)tmp;
        this->total_size += size;
    }
}

int rw_buffer::buffer_append(void *dt, int size)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function buffer_append()\n",pthread_self());
    fflush(stdout);
#endif
    if (dt != NULL)
    {
        make_room(size);
        memcpy(this->data + this->writeIndex, dt, size);
        this->writeIndex += size;
    }
}

int rw_buffer::buffer_append_char(char ch)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function buffer_append_char()\n",pthread_self());
    fflush(stdout);
#endif
    make_room(1);
    this->data[this->writeIndex++] = ch;
}

int rw_buffer::buffer_append_string(char *str)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function buffer_append_string()\n",pthread_self());
    fflush(stdout);
#endif
    if (str != NULL)
    {
        int size = strlen(str);
        buffer_append(str, size);
    }
}

/**
 * 考虑到了原本缓冲区可能不足的情况
 * 使用了iovec结构体和readv函数
 */
int rw_buffer::buffer_socket_read(int fd)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function buffer_socket_read()\n",pthread_self());
    fflush(stdout);
#endif
    char additional_buffer[this->init_buffer_size];
    struct iovec vec[2];
    int max_writable = get_buffer_writeable_size();
    vec[0].iov_base = this->data + this->writeIndex;
    vec[0].iov_len = max_writable;
    vec[1].iov_base = additional_buffer;
    vec[1].iov_len = sizeof(additional_buffer);
    int res = readv(fd, vec, 2);
    if (res < 0)
    {
        return -1;
    }
    else if (res <= max_writable)
    {
        this->writeIndex += res;
    }
    else
    {
        this->writeIndex = this->total_size;
        buffer_append(additional_buffer, res - max_writable);
    }
    return res;
}

int rw_buffer::buffer_socket_send(int fd)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function buffer_socket_send()\n",pthread_self());
    fflush(stdout);
#endif
    int res = send(fd, data + readIndex, writeIndex - readIndex, 0);
    // /* 做清空缓冲区的处理 */
    // memset(this->data,0,sizeof this->data);
    // writeIndex = readIndex = 0;
    return res;
}

char rw_buffer::buffer_read_char()
{
// #ifdef DEBUG
//     printf("pthread: %ld, into to function buffer_read_char()\n",pthread_self());
//     fflush(stdout);
// #endif
    char ch = this->data[this->readIndex];
    this->readIndex++;
    return ch;
}

/**
 * 强耦合，为parse_line()服务 
 * 返回值0：LINE_OK
 * 返回值1：LINE_BAD
 * 返回值2：LINE_OPEN
 */
int rw_buffer::buffer_check_line()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function buffer_check_line()\n",pthread_self());
    fflush(stdout);
#endif
    char temp;
    checked_idx = readIndex;
    for (; checked_idx < writeIndex; ++checked_idx)
    {
        temp = this->data[checked_idx];
        if (temp == '\r')
        {
            if ((checked_idx + 1) == writeIndex)
                return 2; //LINE_OPEN
            else if (this->data[checked_idx + 1] == '\n')
            {
                this->data[checked_idx++] = '\0';
                this->data[checked_idx++] = '\0';
                return 0; //LINE_OK
            }
            return 1; //LINE_BAD
        }
        else if (temp == '\n')
        {
            if (checked_idx > readIndex && this->data[checked_idx - 1] == '\r')
            {
                this->data[checked_idx - 1] = '\0';
                this->data[checked_idx++] = '\0';
                return 0; //LINE_OK
            }
            return 1; //LINE_BAD
        }
    }
    return 2; //LINE_BAD
}

/* 为http_connection的get_line()服务 */
int rw_buffer::get_read_line_char_num()
{
    return this->checked_idx - this->readIndex;
}

/**
 * 找到\r\n的位置
 * 但是memmem函数有问题
 */
// char* rw_buffer::buffer_find_CRLF(){
//     char *crlf = memmem((void*)(this->data + this->readIndex), get_buffer_readable_size(), CRLF, 2);
//     return crlf;
// }