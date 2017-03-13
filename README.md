# epoll_reactor
用epoll实现了一个异步框架, 较为零散和简陋. 仅供学习之用.包含的功能有:

+ 异步tcpserver(用epoll监听socket)
+ 异步post(用epoll监听pipe)
+ 异步timer的框架(用epoll的超时机制).

## 标准的经典的 Reactor模式:

+ 步骤 1) 等待事件 (Reactor 的工作)
+ 步骤 2) 发”已经可读”事件发给事先注册的事件处理者或者回调 ( Reactor 要做的)
+ 步骤 3) 读数据 (用户代码要做的)
+ 步骤 4) 处理数据 (用户代码要做的)

## 模拟的Proactor模式:

+ 步骤 1) 等待事件 (Proactor 的工作)
+ 步骤 2) 读数据(看，这里变成成了让 Proactor 做这个事情)
+ 步骤 3) 把数据已经准备好的消息给用户处理函数，即事件处理者(Proactor 要做的)
+ 步骤 4) 处理数据 (用户代码要做的)

Linux上可以用epoll轻易实现Reactor模型.
boost::asio使用Proactor模型.
我理解的是Proactor不需要用户去调用阻塞读/写函数，比如read/write。 网络库会自动把它读到buffer里面，用户只需要去取buffer就可以了。
