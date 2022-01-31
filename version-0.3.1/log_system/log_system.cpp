#include "log_system.h"

extern log_system *log_system::ls_ptr = NULL;
extern locker log_system::get_instance_mutex;

log_system::log_system(/* args */)
{
}

log_system::~log_system()
{
    if (this->log_fp != NULL)
    {
        fclose(log_fp);
    }
}

/* 用于单例模式 */
log_system *log_system::get_instance()
{
    if (NULL == ls_ptr)
    {
        log_system::get_instance_mutex.lock();
        if (NULL == ls_ptr)
        {
            ls_ptr = new log_system();
        }
        log_system::get_instance_mutex.unlock();
    }
    return ls_ptr;
}

void log_system::init(const char *log_file_name, int buf_size)
{
#ifdef DEBUG_LOG
    printf("pthread: %ld, into to function log_system::init()\n", pthread_self());
    fflush(stdout);
#endif
    /* 在这里创建线程，并进入函数write_log_to_file */
    this->log_queue = new block_queue<std::string>();
    pthread_t pid_t;
    pthread_create(&pid_t, NULL, pthread_called_function, NULL);

    this->log_buf_size = buf_size;
    this->log_buf = new char[this->log_buf_size];
    memset(this->log_buf, '\0', this->log_buf_size);

    this->log_fp = fopen(log_file_name, "a");
}

void *log_system::pthread_called_function(void *data)
{
    log_system::get_instance()->write_log_to_file();
}

/* 将日志信息写入到文件中，线程的调用函数 */
void log_system::write_log_to_file()
{
    std::string log_str;
    while (log_queue->pop(log_str))
    {
        log_mutex.lock();
        fputs(log_str.c_str(), this->log_fp);
        fflush(this->log_fp);
        log_mutex.unlock();
    }
}

/**
 * 加入日志信息，外部业务系统的调用函数 
 * 每天一个日志，根据日期确定日志名字
 */
void log_system::append_log(int level, const char *format, ...)
{
#ifdef DEBUG_LOG
    printf("pthread: %ld, into to function log_system::append_log()\n", pthread_self());
    fflush(stdout);
#endif
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);

    /* 清空缓冲区字符, !!size of 不能用来返回动态创建的数组大小 */
    memset(this->log_buf,0,this->log_buf_size);
    /* 如果是新的一天，则在日志中输入新的日期 */
    log_mutex.lock();
    int day_info_num = 0;
    if((*sys_tm).tm_mday != log_tm.tm_mday){
        //新建一个文件
        day_info_num = snprintf(this->log_buf,this->log_buf_size,"\n\nThe log of %d-%02d-%02d:\n",
        (*sys_tm).tm_year + 1900, (*sys_tm).tm_mon + 1, (*sys_tm).tm_mday);
    }
    log_mutex.unlock();

    /* 更新日期 */
    this->log_tm = *sys_tm;

    /* 初始化要进入阻塞队列的字符串和日志等级字符串 */
    std::string log_str;
    char level_str[15];
    memset(level_str, 0, sizeof level_str); 
    /* 有四种级别，分别是info，debug，warning与error */
    switch (level)
    {
    case 0:
        strcpy(level_str, "[info]:");
        break;
    case 1:
        strcpy(level_str, "[debug]:");
        break;
    case 2:
        strcpy(level_str, "[warning]:");
        break;
    case 3:
        strcpy(level_str, "[error]:");
        break;
    default:
        strcpy(level_str, "[info]:");
        break;
    }

    va_list valst;
    va_start(valst, format);
    log_mutex.lock();

    /* 对输入的字符进行操作 */
    int n = snprintf(log_buf + day_info_num, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                     log_tm.tm_year + 1900, log_tm.tm_mon + 1, log_tm.tm_mday,
                     log_tm.tm_hour, log_tm.tm_min, log_tm.tm_sec, now.tv_usec, level_str);

    int m = vsnprintf(log_buf + day_info_num + n, log_buf_size - 1, format, valst);

    log_buf[day_info_num + n + m] = '\n';
    log_buf[day_info_num + n + m + 1] = '\0';

    log_str = log_buf;
    log_mutex.unlock();
    va_end(valst);
    /* 将处理完的字符串加入到生产者消费者队列中 */
    if (!log_queue->full())
    {
        log_queue->push(log_str);
    }
}