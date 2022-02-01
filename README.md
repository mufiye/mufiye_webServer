# mufiye_webServer
## version 0.1.1
### 需求
这一期的任务是实现基本的TCP服务器框架，要求能较好地解决或接近解决C10K问题（即具有较好的并发处理能力）。
### 设计
多方面考虑，事件处理模式为：主线程监听和建立连接、读取数据、发送数据，子线程（预先创建于线程池中）进行业务处理。（Proactor模式）
### 实现
* 语言：C++
* 类抽象：
  * thread_pool：线程池
  * locker：同步互斥机制封装类
  * acceptor：listen套接字封装类
  * buffer：读写缓冲区
  * tcp_connection：tcp连接封装类
  * simple_connection：在tcp事务处理中代替http_connection
## version 0.2.1
增加了http应用层处理模块，支持简易的GET请求http报文与CGI程序。将connection文件夹中的simple_connection类替换为了http_connection类。
## version 0.3.1
增加一个日志系统，将程序的运行日志记录下来。在log_system文件夹中中添加了日志系统类log_system。
## version 0.4.1
添加定时器容器，处理超时连接。在timer文件夹中添加了定时器类util_timer和定时器容器类sort_timer_lst。
## later version
### rw_buffer读写缓冲区模块
1. 关于的写模块，单纯的send可以替换为writev提高效率
### http处理模块
1. 实现一些有趣的功能
2. 增加POST请求的处理以及响应
3. 增加GET请求参数的处理
4. 设计和实现CGI脚本
### 日志系统模块
1. 日志系统可以考虑多文件
2. 日志系统可以考虑写入的限制
3. perror的信息如何进入日志系统
### 定时器模块
1. 定时器容器的高性能设计方案，如时间轮和时间堆
## 关于事件处理模式
### 事件处理模式的一些其它方案
* **方法一**：“半同步/半异步reactor模式”，异步线程只有一个，由主线程来充当。它负责监听所有socket上的事件。如果监听socket上有可读事件发生，即有新的连接请求到来，主线程就接受之以得到新的连接socket，然后往epoll内核事件表中注册该socket上的读写事件。如果连接socket上有读写事件发生，即有新的客户请求到来或有数据要发送至客户端，主线程就将该连接socket插入请求队列中。所有工作线程都睡眠在请求队列上，当有任务到来时，它们将通过竞争（比如申请互斥锁）获得任务的接管权。
* **方式二**：“高效的半同步/半异步模式”，主线程只管理监听socket，连接socket由工作线程来管理。当有新的连接到来时，主线程就接受之并将新返回的连接socket派发给某个工作线程，此后该新socket上的任何I/O操作都由被选中的工作线程来处理，直到客户关闭连接。
* **自己的设想**：主reactor线程监听端口并负责建立连接，从reactor线程（有多个）负责处理IO事件，处理完的IO事件交给业务线程进行处理。
### 事件处理模式的一些疑问：
* Q1：处理IO事件的线程在tcp连接建立前就阻塞select/poll/epoll上，如何使新连接的套接字高效地被检测？
* Q2：该处理流程是主线程监听端口并建立连接，IO事件交给IO线程处理，业务交给业务线程处理，那么要如何实现主线程、IO线程和业务线程的任务传递呢？
## 一些尝试
### ~~sub-try1（已完成-2022.1.19）~~
* 需求：编写一个多线程程序
* 目的：了解多线程函数的使用
### ~~sub-try2（已完成-2022.1.19）~~
* 需求：编写多线程的服务器端程序以及可以测试多线程服务器程序的客户端程序
* 目的：了解多线程编程，编写出测试类
### ~~sub-try3（已完成-2022.1.20）~~
* 需求：编写使用poll/epoll(select可写可不写)的程序，主线程accpet,子线程IO多路复用处理IO事件
* 目的：了解poll/epoll
### ~~sub-try0（已完成-2022.1.20）~~
* 需求：优化现有的makefile
### ~~sub-try4（已完成-2022.1.21）~~
* 需求：写一个读写buffer，将要用到的读写方法进行封装
### ~~sub-try5（下个版本的任务）~~
* 需求：写一个http请求处理类和http响应处理类，要求分别能够处理http请求报文和生成http响应报文
### ~~sub-try6（已完成-2022.1.21）~~
* 需求：将三个线程同步机制（互斥锁、信号量、条件变量）封装成类
### ~~sub-try7（已完成-2022.1.21）~~
* 需求：写一个生产者消费者队列
### ~~sub-try8（已完成-2022.1.22）~~
* 需求：写一个线程池类
### ~~sub-try9（实现在version-0.1.1中，2022.1.26）~~
* 需求：写一个tcp连接类
### ~~sub-try10（已完成-2022.1.22）~~
* 需求：将服务器的accept功能封装成一个类，使用非阻塞IO
### ~~sub-try11（已完成-2022.1.22）~~
* 关于cgi
### ~~sub-try12（实现在version-0.2.1中）~~
* 需求：使http报文能够请求和传递图片（作为sub-Try5的补充）
## 参考
1. [linux高性能服务器编程（游双）源码](https://github.com/luckyhappycat/high_performance_linux_server_programming)
2. [TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
3. [网络编程实战_极客时间_yolanda](https://github.com/froghui/yolanda)
4. [Tinyhttpd](https://github.com/EZLippi/Tinyhttpd)
5. [WebBench](https://github.com/EZLippi/WebBench)