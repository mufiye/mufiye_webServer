# mufiye_webServer
## Version 0.1.1
### Requirements
### Design
### Implementation
* language: C++
* class:
  * thread_pool: thread pool
  * locker: the class for synchronization and mutual exclusion
  * acceptor: the class for accept the request of connection
  * buffer: the class for reading and writing
  * tcp_connection: the class of tcp connection
  * simple_connection: the class of simple connection take place of http connection
## Version 0.2.1
The HTTP application layer processing module is added to support simple GET request HTTP packets and CGI procedures. I replace the simple_connection class with the http_connection class in the connection folder.  
## Version 0.3.1
I add a log system to record the running log of the program. The logging system class log_system has been added to the log_system folder.  
## Version 0.4.1
I add a timer container to handle timeout connections. The timer class util_timer and the timer container class sort_timer_lst have been added to the Timer folder.  
## Later Version
### the module of reading and writing buffer
1. we can replace the normal send function with writev function to raise efficiency.
### the module of http request processing
1. implement some interesting features
2. add processing and response module of POST requests
3. add processing of GET request parameters
4. design and implementation of CGI scripts
### the module of logging system
1. we can consider multiple files of logging system
2. we can consider writing limitation of logging system
3. we can try to add the information of perror to the logging system
### the module of timer
1. high performance design solutions for timer containers, such as time wheel and time heap
## Reference
1. [linux高性能服务器编程（游双）源码](https://github.com/luckyhappycat/high_performance_linux_server_programming)
2. [TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
3. [网络编程实战_极客时间_yolanda](https://github.com/froghui/yolanda)
4. [Tinyhttpd](https://github.com/EZLippi/Tinyhttpd)
5. [WebBench](https://github.com/EZLippi/WebBench)
