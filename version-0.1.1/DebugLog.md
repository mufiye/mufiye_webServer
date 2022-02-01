# Debug日志
## 2022.1.26
### 1. multiple definition
#### 原因
原因是将变量、函数声明和定义都放在头文件之中，但是头文件被不同文件多次导入，导致变量或函数被多次定义。
#### 解决方案
将变量和函数定义与声明分开，定义放在cpp文件中，声明放在头文件中。
### 2. 未找到类静态变量的定义
#### 原因
类静态变量需要在声明后单独定义。
#### 解决方案
由于如果在头文件中定义会被重复引入导致多次定义，将定义放在cpp文件中，用类名加::限定符标识。
### 3. 偶尔会“Assertion `connFd >= 0' failed.”
#### 原因
accept函数出现“Invalid Argument”错误。
#### 解决方案
getConnFd()中的clientLen定义为sizeof(clientAddr)，clientLen是accept函数的第3个参数。

# over
