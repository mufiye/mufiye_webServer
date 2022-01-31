/**
 * 日志系统
 * Q1：日志写在什么格式的文件里
 * A1：自定义的.log
 * Q2：如何高效地写（因为只有一个日志系统和日志文件，多线程运行时写时会阻塞、严重得甚至会影响正常的处理流程）
 * A2：新建一个线程
 * Q3：日志系统写的内容是什么？
 * A3：多个级别的打印信息
 */
#ifndef LOG_SYSTEM_H
#define LOG_SYSTEM_H

#include "block_queue.h"

class log_system
{
private:
    /* 用于单例模式 */
    static locker get_instance_mutex;
    static log_system *ls_ptr;

    /* 文件及其描述符 */
    std::string path_name;
    std::string file_name;
    FILE *log_fp;

    /* 字符缓冲区 */
    char *log_buf;
    int log_buf_size;

    /* 阻塞队列 */
    block_queue<std::string>* log_queue;
    locker log_mutex;

    // /* 关于时间，用于日志文件的划分 */
    struct tm log_tm;

private:
    log_system();
    ~log_system();
    /* 将日志信息写入到文件中 */
    static void* pthread_called_function(void* data);
    void write_log_to_file();
    
public:
    /* 用于单例模式 */
    static log_system *get_instance();
    /* 进行日志系统的初始化 */
    void init(const char*,int);
    /* 加入日志信息，外部业务系统的调用函数 */
    void append_log(int level, const char *format, ...);
};


#define LOG_INFO(format, ...) log_system::get_instance()->append_log(0, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) log_system::get_instance()->append_log(1, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) log_system::get_instance()->append_log(2, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) log_system::get_instance()->append_log(3, format, ##__VA_ARGS__)

#endif