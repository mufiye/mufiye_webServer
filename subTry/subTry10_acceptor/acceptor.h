#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <fcntl.h>

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
