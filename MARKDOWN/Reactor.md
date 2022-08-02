## Muduo的事件处理的关键结构

### 事件分发：Channel

> 负责一个`fd`上面IO事件的分发，`Channel`将不同的IO事件进行回调(socket, eventfd, timbered, signaled)
>
> `Channel`的生命周期由`owner`负责

**具体的流程**

> 给定`Channel`的`loop`和需要处理的`fd`,通过`enableReading`进行注册读事件。
>
> `enableReading()` -> `Channel::update()`->`EventLoop::updateChannel`->`Poller::updateChannel`
>
> 最终向poll系统调用的监听事件做修改

