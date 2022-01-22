#include "acceptor.h"

int main()
{
    acceptor ap(10000);
    char buf[1024];
    int len;
    while (1)
    {
        //printf("in\n");
        int connFd = ap.getConnFd();
        if (connFd < 0)
            continue;
        printf("the new connFd is %d\n", connFd);
        memset(&buf, 0, sizeof buf);
        sprintf(buf, "Hello, I'm server");
        send(connFd, buf, strlen(buf), 0);
        close(connFd);
    }
    return 0;
}