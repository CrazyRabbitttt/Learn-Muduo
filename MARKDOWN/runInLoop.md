# RunInloop

## 作用

任何一个线程，只要创建并运行了EventLoop，都称之为IO线程。runInLoop()使得IO线程能够执行某个用户任务回调。
如果用户在当前IO线程调用这个函数，回调会同步进行；如果用户在其他线程调用runInLoop()，回调函数会加入到队列中，IO线程会被唤醒来调用这个函数。
这样就能够轻易地在线程间调配任务，比如将回调函数都移到IO线程中执行，就可以在不使用锁的情况下保证线程安全。

## 任务

> - EventLoop中需要包含wakeupChannel_通道唤醒它
> - 当mainLoop获取一个新用户的Channel，通过轮询算法选择一个subLoop，通过该成员变量唤醒subLoop。
