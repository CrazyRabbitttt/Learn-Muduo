#ifndef BING_NET_EVENTLOOP_H
#define BING_NET_EVENTLOOP_H

#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/CurrentThread.h"
#include "Learn-Muduo/Base/TimeStamp.h"
#include "Learn-Muduo/Timer/TimerQueue.h"

#include <vector>

namespace bing {

class Channel;          //前向声明
class Poller;

class EventLoop : nocopyable
{
public:

    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    // 下面是定时器部分
    void RunAt(TimeStamp timestamp, Functor cb) {
        timer_queue_->AddTimer(timestamp, std::move(cb), 0.0);  
    }

    void RunAfter(double wait_time, Functor cb) {
        // 获得等待一段时间之后的时间戳
        TimeStamp timestamp(TimeStamp::AddTime(TimeStamp::now(), wait_time));
        RunAt(timestamp, std::move(cb));
    }

    void RunEvery(double interval, Functor cb) {
        TimeStamp timestamp(TimeStamp::AddTime(TimeStamp::now(), interval));
        timer_queue_->AddTimer(timestamp, std::move(cb), interval);
    }



    // 执行事件循环
    void loop();
    //退出事件循环
    void quit();

    //返回事件的时候的事件点
    TimeStamp epollReturnTime() const  { return pollReturnTime_; }

    //当前loop执行回调函数
    void runInLoop(const Functor& cb);

    //将回调放入队列，线程被唤醒后执行
    void queueInLoop(const Functor& cb);

    //进行唤醒，唤醒loop
    void wakeup();

    //会进行事件的注册，向epoller传递
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);


    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() { return threadId_ == bing::currentThread::tid(); }


private: 

    //wake up, 处理进行唤醒的读事件
    void handleRead();  

    // 执行回调函数
    void doPendingFunctors();   

    using ChannelList = std::vector<Channel*>;
    void abortNotInLoopThread();        //警告不是在本线程中·

    int wakeupFd_;                      //进行唤醒的事件描述符号
                                        //当mainLoop获取一个新用户的Channel，通过轮询算法选择一个subLoop，通过该成员变量唤醒subLoop。

    std::atomic_bool callingPengdingChannel_;   //正在唤醒
    bool looping_;
    bool quit_;

    TimeStamp pollReturnTime_;                  //Epoller 返回发生事件的时间点  
    std::unique_ptr<Channel> wakeupChannel_;    //标志：当前Loop是否有需要执行的回调操作
    std::vector<Functor> pendingFunctors_;      //loop需要执行的所有的回调函数
    MutexLock mutex_;                           //保护vector
    std::unique_ptr<Poller> poller_;            //one loop, one poller 
    std::unique_ptr<TimerQueue> timer_queue_;   
    ChannelList activeChannels_;
    const pid_t threadId_;          //线程标识

};


}  //namespace bing 

#endif