### examples

#### TCP demos
- 使用handy的示例
- 
- echo.cc 简单的回显服务 [OK]
- timer.cc 使用定时器来管理定时任务 [OK]
- idle-close.cc 关闭一个空闲的连接 [OK]
- reconnect.cc 设置连接关闭后自动重连 [OK]
- safe-close.cc 在其他线程中安全操作连接 [OK]
- chat.cc 简单的聊天应用，用户使用telnet登陆后，系统分配一个用户id，用户可以发送消息给某个用户，也可以发送消息给所有用户 [OK]
- codec-cli.cc 发送消息给服务器，使用的消息格式为mBdT开始，紧接着4字节的长度，然后是消息内容 [OK]
    - mBdT： 4byte长度+msg
- codec-svr.cc 见上 [OK]
- hsha.cc 半同步半异步示例，用户可以把IO交给handy框架进行处理，自己同步处理用户请求 [OK]
    - 比较hsha和非hsha的区别？
        - 非hsha需要用户处理tcp的状态变更、事件等操作 [OK]
- http-hello.cc 一个简单的http服务器程序
    - MultiBase实现模型
- stat.cc 一个简单的状态服务器示例，一个内嵌的http服务器，方便外部的工具查看应用程序的状态[ok]
- write-on-empty.cc 这个例子演示了需要写出大量数据，例如1G文件这种情景中的使用技巧 [ok]
- daemon.cc 程序已以daemon方式启动，从conf文件中获取日志相关的配置，并初始化日志参数  [ok]

#### UDP demos
- udp-cli.cc udp的客户端
- udp-svr.cc udp服务器
- udp-hsha.cc udp的半同步半异步服务器


#### TODO
- EventBase EventBases MultiBase的关系
    - EventBase ： EventBases
    - MultiBase ： EventBases
    - MultiBase ： vector<EventBase> bases_

