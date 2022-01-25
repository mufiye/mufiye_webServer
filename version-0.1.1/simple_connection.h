/**
 * @file simple_connection.h
 * @author mufiye (1433147434@qq.com)
 * @brief 
 * @version 0.1.1
 * @date 2022-01-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * tcp_connection的继承类？或者说是包含tcp_connection类
 */
#ifndef SIMPLE_CONNECTION_H
#define SIMPLE_CONNECTION_H

#include "tcp_connection.h"

class simple_connection : public tcp_connection
{
private:
    /* data */
public:
    simple_connection(/* args */);
    ~simple_connection();
    void process();
};

#endif