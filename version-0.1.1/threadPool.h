/**
 * @file threadPool.h
 * @author from TinyWebServer
 * ------------------------------------------
 * Q1：为什么worker()要写为一个静态函数
 * A1：因为pthread_create()参数传递的问题
 * Q2：为什么worker()和run()这两个函数要分开写？
 * A2：因为worker()作为静态成员函数不能访问普通成员变量
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "locker.h"


template <typename T>
class threadPool
{
private:
    int m_thread_number;
    int m_max_requests;
    pthread_t *m_threads;
    std::list<T *> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat;
    bool m_stop;

private:
    static void *worker(void *arg); /* 这个函数必须是静态函数 */
    void run();

public:
    threadPool(int thread_number = 4, int max_request = 10000);
    ~threadPool();
    bool append(T *request);
};

template <typename T>
threadPool<T>::threadPool(int thread_number, int max_requests) : m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false), m_threads(NULL)
{
    if (thread_number <= 0 || max_requests <= 0)
        throw std::exception();
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
        throw std::exception();
    for (int i = 0; i < thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
threadPool<T>::~threadPool()
{
    delete[] m_threads;
    m_stop = true;
}

template <typename T>
bool threadPool<T>::append(T *request)
{
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void *threadPool<T>::worker(void *arg)
{
    threadPool *pool = (threadPool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void threadPool<T>::run()
{
    while (!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if(!request)
            continue;
        request->process();
    }
}
#endif