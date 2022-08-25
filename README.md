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






