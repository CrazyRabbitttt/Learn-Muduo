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
