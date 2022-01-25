/**
 * from tinyWebServer
 *
 * 封装之后使其资源获取即初始化
 * 变量回收则资源销毁
 */
#ifndef LOCKER_H
#define LOCKER_H

#include "common.h"

/**
 * 对三个线程同步机制封装类
 * 互斥锁 locker
 * 信号量 sem
 * 条件变量 cond
 */

class locker
{
private:
    pthread_mutex_t m_mutex;

public:
    locker();
    ~locker();
    bool locker::lock();
    bool locker::unlock();
    pthread_mutex_t *get();
};

class sem
{
private:
    sem_t m_sem;

public:
    sem();
    sem(int num);
    ~sem();
    bool wait();
    bool post();
};

class cond
{
private:
    pthread_cond_t m_cond;

public:
    cond();
    ~cond();
    bool wait(pthread_mutex_t *m_mutex);
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t);
    bool signal();
    bool broadcast();
};

#endif