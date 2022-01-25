#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "common.h"

const int LISTENQ = 10000;
class acceptor
{
private:
    int listenFd;
    int listenPort;
public:
    acceptor(int listenPort);
    int getConnFd();
    int getListenFd();
    int getListenPort();
    ~acceptor();
};

#endif
