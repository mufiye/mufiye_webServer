/**
 * 缓冲区
 * from yolanda
 */

#ifndef RW_BUFFER_H
#define RW_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>

const char *CRLF = "\r\n";
const int INIT_BUFFER_SIZE = 65536;

class rw_buffer
{
private:
    char *data;     //实际缓冲
    int readIndex;  //缓冲读取位置
    int writeIndex; //缓冲写入位置
    int total_size; //总大小
public:
    rw_buffer(/* args */);
    ~rw_buffer();
    int get_buffer_writeable_size();
    int get_buffer_readable_size();
    int get_buffer_front_spare_size();
    void make_room(int size);
    int buffer_append(void *data, int size);
    int buffer_append_char(char data);
    int buffer_append_string(char *data);
    int buffer_socket_read(int fd);
    char buffer_read_char();
    char *buffer_find_CRLF();
};

#endif