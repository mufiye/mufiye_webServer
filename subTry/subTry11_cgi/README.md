## 关于cgi
* 可以查看tinyhttpd/httpd.c文件的execute_cgi()函数
* 大致就是fork出一个子进程，子进程运行程序并将结果通过管道传给父进程
* 问题
  * Q1：.cgi程序是怎样的程序？如何书写？
  * A1：
  * Q2：