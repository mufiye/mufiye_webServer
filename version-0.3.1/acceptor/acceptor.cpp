#include "acceptor.h"
#include "../log_system/log_system.h"

log_system* logSystem = log_system::get_instance();

acceptor::acceptor(int listenPort)
{
    this->listenPort = listenPort;
    this->listenFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    //将套接字设置为非阻塞IO
    fcntl(this->listenFd, F_SETFL, O_NONBLOCK);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(this->listenPort);

    int on = 1;
    setsockopt(this->listenFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(this->listenFd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        perror("bind failed");
    }

    int rt2 = listen(this->listenFd, LISTENQ);
    if (rt2 < 0) {
        perror("listen failed");
    }
}

acceptor::~acceptor()
{
}

int acceptor::getConnFd(){
    //客户端地址信息如何处理？
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int connFd = accept(this->listenFd,(sockaddr*)&clientAddr,&clientLen);
    if(connFd < 0){
        perror("some wrong with accept()");
    }
    LOG_INFO("the connFd is %d\n",connFd);
    return connFd;
}

int acceptor::getListenFd(){
    return this->listenFd;
}

int acceptor::getListenPort(){
    return this->listenPort;
}
