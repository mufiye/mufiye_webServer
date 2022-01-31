# version-0.3.1的DEBUG日志
## 2022.1.30
### 1. default argument given...
#### 错误提示
error: default argument given for parameter 1 of ‘rw_buffer::rw_buffer(int)’ [-fpermissive]
#### 原因
既可以在类的声明中，也可以在函数定义中声明缺省参数，但不能既在类声明中又在函数定义中同时声明缺省参数。


## 未来展望
1. 日志系统可以考虑多文件
2. 日志系统可以考虑写入的限制
3. perror的信息如何进入日志系统