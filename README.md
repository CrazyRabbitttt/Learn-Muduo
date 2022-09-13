### 高性能WebServer 

项目是基于CPP11编写的WebServer，以OneLoopPerThread为线程模型，以事件驱动模式作为核心。
支持HTTP的GET请求连接，并且实现了简单版本的Proxy Server来减缓服务器的压力。
实现了定时器断开空闲连接的功能与日志库进行Logging的功能。

### Optimized Methods

●OneLoopPerThread线程模型实现高性能服务器，同开源项目中性能相差无几 
●状态机进行HTTP请求的解析，支持GET， HEAD方法 
●使用小根堆管理定时器，支持长连接和定时回调超时连接 
●异步高性能日志库完成了服务器的logging功能 
●使用线程池管理线程，减少了线程创建和销毁的性能损失 
●增设Proxy代理服务器减轻主服务器的压力 
●使用多Reactor多线程模型（epoll），实现了负载均衡与解耦 
●使用读写锁模型维护Proxy中Cache的访问，降低了锁粒度，优化了客户端响应的时间 
●使用RAII封装同步机制(mutex,cond)和管理文件描述符 
●使用智能指针unique_ptr等管理指针，防止内存泄漏 


### 同类项目的性能对比
```sh

# 由于使用的是阿里云服务器测试，性能太低，就只用5000并发测试。
# 但是变量唯一嘛，同其他开源高性能项目进行对比

# 本项目在5000并发下的测试
root@iBing:~/Learn-Muduo/webbench# ./webbench -t 30 -c 5000 http://127.0.0.1:9090/
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:9090/
5000 clients, running 30 sec.

Speed=195020 pages/min, 6276329 bytes/sec.
Requests: 97510 susceed, 0 failed.
=======================================================================================================================================

# 其他项目的性能测试
root@iBing:~/Learn-Muduo/webbench# ./webbench -t 30 -c 5000 http://127.0.0.1:9090/
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:9090/
5000 clients, running 30 sec.

Speed=169120 pages/min, 5561451 bytes/sec.
Requests: 84435 susceed, 125 failed.
```
> 测试了几次，使用阿里云测试的，可能是由于网络问题测试有些偏差，结果有时候高有时候低一点，不过都差距不太大


### 运行环境

- Linux Version >= 2.6
- Ubuntu/CentOS..

### 编译环境

> - g++, cmake, make

### 编译运行

> ./build.sh
