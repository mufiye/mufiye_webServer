/**
 * 缓冲区
 * from yolanda
 */

#ifndef RW_BUFFER_H
#define RW_BUFFER_H

#include "../common/common.h"


class rw_buffer
{
private:
    char *data;     //实际缓冲
    int readIndex;  //缓冲读取位置
    int writeIndex; //缓冲写入位置
    int init_buffer_size; //初始的容器大小
    int total_size; //总大小
    int checked_idx; //服务于http,很抱歉加强了耦合性
    //int content_size; //内容的大小

public:
    rw_buffer(int size = 256);
    ~rw_buffer();
    void reset();
    char* get_buffer_data();
    int get_buffer_read_index();
    int get_buffer_write_index();
    int get_buffer_writeable_size();
    int get_buffer_readable_size();
    int get_buffer_front_spare_size();
    int get_buffer_content_size();
    void make_room(int size);
    int buffer_append(void *data, int size);
    int buffer_append_char(char data);
    int buffer_append_string(char *data);
    int buffer_socket_read(int fd);
    int buffer_socket_send(int fd);
    char buffer_read_char();
    char *buffer_find_CRLF();
    int buffer_check_line();
    int get_read_line_char_num();
};

#endif