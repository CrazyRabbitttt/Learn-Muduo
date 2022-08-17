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






