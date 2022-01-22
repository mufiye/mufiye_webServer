#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../subTry10_acceptor/acceptor.h"

const int STDIN = 0;
const int STDOUT = 1;
const int STDERR = 2;

int main()
{
    int status;

    /* 两个管道 */
    int cgi_input[2];
    int cgi_output[2];
    pipe(cgi_input);
    pipe(cgi_output);

    pid_t pid = fork();
    if (pid > 0)
    { /* parent */
        close(cgi_output[1]);
        close(cgi_input[0]);

        //连接设置
        acceptor ap(10000);
        int connFd;
        while (1)
        {
            connFd = ap.getConnFd();
            if (connFd < 0)
                continue;
            printf("the new connFd is %d\n", connFd);
            break;
        }

        char buf[2048];
        memset(&buf,0,sizeof buf);
        read(cgi_output[0], buf, 2048);
        printf("the child write: %s\n", buf);
        send(connFd, buf, strlen(buf), 0);
        close(connFd);

        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0); //等待子进程结束
    }
    else if (0 == pid)
    { /* child: run the program */
        dup2(cgi_output[1], STDOUT);
        dup2(cgi_input[0], STDIN);
        close(cgi_output[0]);
        close(cgi_input[1]);
        //char buf[1024];
        //sprintf(buf,"hello,I'm the child's program");
        //write(STDOUT,buf,strlen(buf));

        /* run the program */
        /* 如果exec那么管道就不用关闭了 */
        execl("./cgi_scripts/demo.cgi", NULL); //运行path这个文件
        exit(0);
    }
    else
    {
        perror("some wrong when fork");
    }
}