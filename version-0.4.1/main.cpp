/**
 * @file main.cpp
 * @author mufiye (1433137434@qq.com)
 * @brief 
 * @version 0.1.1
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * 主线程文件
 */
#include "./acceptor/acceptor.h"
//#include "./locker/locker.h"
#include "./buffer/rw_buffer.h"
#include "./threadPool/threadPool.h"
#include "./connection/http_connection.h"
#include "./log_system/log_system.h"
#include "./timer/lst_timer.h"

const int MAX_EVENT_NUMBER = 4096;
extern void addfd(int, int, bool);
extern void removefd(int, int);

/* 关于处理超时连接 */
#define TIMESLOT 5
static int pipeFd[2];
static sort_timer_lst timer_lst;
static int epollFd = 0;
std::map<int, util_timer *> timer_mp;

void sig_handler(int sig)
{
    LOG_INFO("into sig_handler()\n");
    int save_errno = errno;
    int msg = sig;
    send(pipeFd[1], (char *)&msg, 1, 0);
    errno = save_errno;
}

void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    //sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void timer_handler()
{
    timer_lst.tick();
    alarm(TIMESLOT);
}

void ot_func(tcp_connection *tcp_conn)
{
    assert(tcp_conn);
    int tempFd = tcp_conn->getConnFd();
    timer_mp.erase(tempFd);
    removefd(epollFd,tempFd);
    delete tcp_conn;
    LOG_INFO("close connection, connection fd is %d\n", tempFd);
}

int main(int argc, char *argv[])
{
    /* 日志系统初始化 */
#ifdef DEBUG_LOG
    printf("pthread: %ld, into to function main()\n", pthread_self());
    fflush(stdout);
#endif
    log_system::get_instance()->init("./log/mufiye_webServer.log", 256);
    LOG_INFO("pthread: %ld, into to function main()\n", pthread_self());

    int port;     //监听的端口号
    int listenFd; //监听套接字
    epoll_event events[MAX_EVENT_NUMBER];
    /* 创建线程池 */
    threadPool<tcp_connection> *pool = new threadPool<tcp_connection>();
    bool server_stop = false;
    if (argc <= 1)
    {
        LOG_ERROR("usage: input port_number\n");
        return 1;
    }

    port = atoi(argv[1]);
    acceptor ap(port);
    listenFd = ap.getListenFd();
    assert(listenFd >= 0);
    epollFd = epoll_create1(0);
    assert(epollFd != -1);
    tcp_connection::set_epollFd(epollFd);
    addfd(epollFd, listenFd, false);

    /* 该套接字用于处理超时连接 */
    int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipeFd);
    assert(ret != -1);
    setnonblocking(pipeFd[1]);
    addfd(epollFd, pipeFd[0], false);

    addsig(SIGALRM);
    addsig(SIGTERM);
    bool timeout = false;
    alarm(TIMESLOT);

    while (!server_stop)
    {
        LOG_INFO("into main while loop\n");
        int number = epoll_wait(epollFd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            LOG_ERROR("epoll failure\n");
            break;
        }else if(number < 0 && errno == EINTR){
            LOG_INFO("interrupted by signo\n");
        }
        //printf("the number is %d\n",number);
        for (int i = 0; i < number; i++)
        {
            LOG_INFO("into get epoll for-loop\n");
            int sockFd = events[i].data.fd;
            if (sockFd == listenFd)
            {
                /* 做accept操作 */
                int connFd = ap.getConnFd();
                //printf("the connFd is %d\n",connFd);
                /* 优化点：获取客户端地址信息？*/
                assert(connFd >= 0);
                tcp_connection *http_conn = new http_connection();
                http_conn->setConnFd(connFd);
                tcp_connection::insert_tcp_conn(connFd, http_conn);
                addfd(epollFd, connFd, true);
                util_timer *timer = new util_timer;
                timer->user_connection = http_conn;
                timer->ot_func = ot_func;
                time_t cur = time(NULL);
                timer->expire = cur + 3 * TIMESLOT;
                timer_lst.add_timer(timer);
                timer_mp.insert(std::pair<int, util_timer *>(connFd, timer));
                continue;
            }
            else if ((sockFd == pipeFd[0]) && (events[i].events & EPOLLIN))
            {
                LOG_INFO("into sockerFd == pipeFd[0]\n");
                int sig;
                char signals[1024];
                ret = recv(pipeFd[0], signals, sizeof(signals), 0);
                if (ret == -1)
                {
                    LOG_ERROR("some thing error with recv the info of pipeFd");
                    continue;
                }
                else if (ret == 0)
                {
                    continue;
                }
                else
                {
                    for (int j = 0; j < ret; j++)
                    {
                        switch (signals[j])
                        {
                        case SIGALRM:
                        {
                            LOG_INFO("into SIGALRM case\n");
                            timeout = true;
                            break;
                        }
                        case SIGTERM:
                        {
                            LOG_INFO("into SIGTERM case\n");
                            server_stop = true;
                            break;
                        }
                        }
                    }
                }
            }
            else if (events[i].events & EPOLLIN)
            {
                LOG_INFO("into EPOLLIN\n");
                int connFd = events[i].data.fd;
                tcp_connection *tcp_conn = tcp_connection::get_tcp_conn(connFd);
                util_timer *timer = timer_mp[connFd];
                /* 读取数据 */
                if (tcp_conn->read_data())
                {
                    LOG_INFO("has read data\n");
                    pool->append(tcp_conn);
                    if (timer)
                    {
                        time_t cur = time(NULL);
                        timer->expire = cur + 3 * TIMESLOT;
                        LOG_INFO("%s", "has adjust timer");
                        timer_lst.adjust_timer(timer);
                    }
                }
                else
                {
                    timer->ot_func(tcp_conn);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                LOG_INFO("into EPOLLOUT\n");
                int connFd = events[i].data.fd;
                tcp_connection *tcp_conn = tcp_connection::get_tcp_conn(connFd);
                util_timer *timer = timer_mp[connFd];
                /* 发送数据 */
                if (tcp_conn->send_data() > 0)
                {
                    LOG_INFO("I have send data\n");
                    if (tcp_conn->isLinger()) //如果是keep-alive
                    {
                        tcp_conn->reset();
                        modfd(epollFd, tcp_conn->getConnFd(), EPOLLIN);
                        if (timer)
                        {
                            time_t cur = time(NULL);
                            timer->expire = cur + 3 * TIMESLOT;
                            LOG_INFO("%s", "has adjust timer");
                            timer_lst.adjust_timer(timer);
                        }
                    }
                    else
                    {
                        timer->ot_func(tcp_conn);
                        if (timer)
                        {
                            timer_lst.del_timer(timer);
                        }
                    }
                }
                else
                {
                    timer->ot_func(tcp_conn);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
        }
        if (timeout)
        {
            timer_handler();
            timeout = false;
        }
    }
    close(listenFd); // !!应当封装在acceptor中
    close(epollFd);
    close(pipeFd[1]);
    close(pipeFd[0]);
    std::map<int, util_timer *>::iterator it = timer_mp.begin();
    for(;it!=timer_mp.end();++it){
        timer_mp.erase(it);
    }
    tcp_connection::remove_all_tcp_conn();
    delete pool;
    /* 相应资源的回收！！ */
    return 0;
}