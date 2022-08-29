<div align="center">  
  <img  src="https://github-readme-streak-stats.herokuapp.com?user=CrazyRabbitttt&theme=onedark&date_format=M%20j%5B%2C%20Y%5D" />
</div>

![Dong Yuanwai's GitHub stats](https://github-readme-stats.vercel.app/api?username=CrazyRabbitttt&show_icons=true)
### 事件总体的改变的趋势

> `Channel update remove => EvnentLoop updateChannel removeChannel => Poller updateChannel removeChannel`
>
> - 某个地方调用Channel的`update`等
> - `Channel`所属的`EventLoop`对`channel`进行操作
> - `EventLoop`**调用底层的IO复用类：epoll**
>
> **其中Channel中的很多的状态信息：index、fd等用于辅助epoll对其的操作**

### 创建Epoller

> 这里将Epoller作为是默认的Poller， 创建默认的这个函数在一个单独的头文件进行:`Newdafault...`


## 新连接建立

### accept执行回调(TcpServer::newConnection)

> - 从线程池中拿出一个线程`ioLoop`作为处理的循环。
> - 创建`Tcpconnection`, 设置一下`connection`的各种回调函数（用户自定义的函数）



## HTTP各个部位的作用

#### Request（解析Method， Headers， 请求行）

> 其中保存了用户http请求报文的各个字段： `method`, `version`, `path`等。
>
> **这个类作为主要的报文解析器, 报文的解析工作就在这里执行**
>
> - ParseMethod : 传入start, end获得方法
> - ParseHeaders ： 同上，获得头部的字段
> - ParseBodys ： 暂时没处理，处理POST请求
> - ParseRequestLine：**统领上面的处理Method方法， 处理一条请求报文行**
>
> 其中主要是在`ParseLine`中进行调用`ParseMethod`, 其中用了`find, move`函数加快效率

#### Response

> 其中内含了`HTTP响应报文的各个字段`， 最终通过`APPEND`进行整合成为响应报文。

#### Content（主状态机）

> - **ParseLine：**   从`Buffer`中提取出来一行消息（\r\n作为分隔符）， 然后进行处理
> - **ParseContent**：主状态机的变化，操控着对应的读取、解析

## **解析的流程**

> **消息的接收：** 毫无疑问是`MessageCallback` **从对端socket中读取出来放到buffer中的**
>
> **请求报文的获取、解析**： 在`MessageCallback`中我们用content去操作这个buffer, 解析的过程中内部会将`HTTP请求报文进行解析，贮存，生成响应报文`
>
> **桥梁：**在`TcpConnection`中存储一个`content`实例



## 🐛：

> #### **不能够解析`HTTP`报文**
>
> - `httpserver`中不要`retrieveallasstring`来看收到了什么，这会清空接收缓冲区从而不能解析了（小🐛）
> - 在解析一行中`ParseLine`, 最后`state_line`的状态赋值 **多打了一个等于号**（小🐛）
> - 解析版本的时候将1写成了0（小🐛）
> - 请求连接后没有直接关闭连接，发送回车服务端异常断开（read=0触发closecallback）: 内部调用closecallback失败，已经解决 （中🐛）
>
> #### **主动关闭连接，如何确保对端收到数据？**
>
> **问题1:如果不是长连接的话，发送完毕数据之后应该是关闭连接，但是 数据没发送完连接就关闭了**
>
> 1. 暂时使用`sleep(1)`等待数据发送完毕是可以的，但是这是绝对不允许的。用其他 **同步机制？ or 别的方法**
> 2. 
>
> 



> - 在写`Accept类`遇到一个问题：这个类创建、监听listenfd, 往`epoll`上面注册读事件，读事件触发(客户端进行连接触发读事件)了之后就调用回调函数（accpet + 执行用户传入的函数）
>
> **测试程序**
>
> - 测试程序的回调函数：触发了`listenfd`读事件之后会内部执行`accept`建立连接，测试的回调函数会发送 **How are you?**, 然后关闭掉。
>
> **现象：**
>
> - 直接卡住，服务端显示没有捕捉到listenfd的读事件，但是`netstat`显示连接建立？？？？？
> - 如果用gdb一步步执行会捕捉到读事件，然后回送给客户端这句话
>
> **结果：**
>
>  我懵了，我还是太菜了😩


### 性能测试(阿里云1核2G)



### 本机5000并发60秒(21W), 4000并发100秒（29W）

```shell
root@iBing:~/Learn-Muduo/webbench# ./webbench -c 5000 -t 60 http://127.0.0.1:9090/index
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:9090/index
5000 clients, running 60 sec.

Speed=219350 pages/min, 7059511 bytes/sec.
Requests: 219350 susceed, 0 failed.
```

> - 吃CPU：37， 7
> - 成功的个数比较多，0 failed,应该是还没有写日志的原因



### 带日志的网络上的服务器，16W，26W（800失败）

```shell
root@iBing:~/Learn-Muduo/webbench# ./webbench -c 5000 -t 60 http://127.0.0.1:9090/
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:9090/
5000 clients, running 60 sec.

Speed=169371 pages/min, 5561748 bytes/sec.
Requests: 168879 susceed, 492 failed.
```

> 吃CPU ： 27， 3.5
>
> 可能是输出日志的工作占比是比较严重的



### strace 进行比较

> 4000并发 ➕ 运行10秒

```shell
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 76.20    0.683423          13     51924           write
  6.65    0.059615           5     12981           close
  6.12    0.054881           4     12981           accept4
  4.76    0.042659           2     25962           setsockopt
  2.70    0.024252           2     12983           epoll_wait
  2.22    0.019949           2     12981           getpeername
  1.35    0.012137           2      6490           read
------ ----------- ----------- --------- --------- ----------------
100.00    0.896916                136302           total
```

> 下面是我的

```shell
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 79.27    0.998183          29     34378           write
  6.19    0.077918           5     16261           close
  6.11    0.076926           4     17189           accept4
  4.81    0.060573           4     17239           epoll_wait
  2.37    0.029840           2     17189           getsockname
  1.24    0.015556           2      7579           read
  0.02    0.000193           6        31           brk
------ ----------- ----------- --------- --------- ----------------
100.00    1.259189                109866           total
~                                                                
```

#### 定时器的设计

> - 基于小根堆进行设计，每次的`tick`时间都是距离超时时间最短的那个定时器的时间，这样就能够保证所有的定时器都能够被`tick`
> - `timerfd` & `gettimeofday`, 使用超时文件描述符能够很好的契合`Reactor`模式进行事件的处理，`gettimeofday`精度很小，能够很好的适配定时器
> - 为每一个连接`connection`创建 **时间戳：最近的更新时间**， 当有消息到达的时候更新这个值，那么`HandleIdleconnection`就不会将连接关闭
> - 超时的时候触发读事件的回调函数，这时候 **将超时的timer进行回调函数的处理**， 然后将 **最小堆（按照超时时间戳进行排序）， 最近触发的定时器的时间作为时间间隔重新设置**



