# Debug日志
## 2022.1.28
### 1. Segmentation fault
### 2. 内容无故消失
因为char指针类型不安全，将char类型替换成了C++标准库的string类型，虽然这样增加了开销。
### 3. 发现tcp的连接经常没有很好地关闭（涉及对象销毁函数的设计）
待解决
### 4. parse_line之后，get-line有时候会没读全
已解决，当初这个函数this->input_buffer->get_read_line_char_num()有问题。
### 5. 让其顺利地进行parse_header()
已解决。
### 6. parse_content()如何进行
这与未来展望1相关
### 7. http连接的keep-alive属性如何对待？
目前尝试总是保活连接？之后再加定时器的功能处理超时连接（超时指连接建立很长一段时间但是无响应）。

## 使用的工具或Debug方式
1. gdb
2. tcpdump
3. wireshark：筛选报文命令为tcp.port == 8080 && ip.addr == 1.15.32.184
4. 每个函数入口打印线程号和函数名（宏定义Debug才允许打印）

## 未来展望
1. POST请求的处理
2. 如何发送html文件（以高效的方式）
3. cgi程序的运行与对应报文的发送
 