#include "Learn-Muduo/Net/EventLoopThread.h"
#include "Learn-Muduo/Net/EventLoop.h"

#include <functional>

using namespace bing;


EventLoopThread::EventLoopThread() 
    :loop_(NULL),
     exiting_(false),
     mutex_(),
     cond_(mutex_),
     thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread")
     {}


EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        //退出EventLoop
        loop_->quit();
        //等待子线程退出
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    //启动新的线程，运行threadfunc,创建EventLoop
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while (loop_ == NULL) cond_.wait();     //等待threadFunc真正跑起来
    }

    //构造的时候创建的loop_传回去，已经开启了loop了
    return loop_;
}

//线程的实际工作是创建EventLoop，one loop per thread 
void EventLoopThread::threadFunc() {

    EventLoop loop;

    // if (callback_) callback_(&loop);
    //修改loop_
    {   
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();         //通知线程， 拿到了loop_
    }

    //loop对象的生命周期是和主函数的周期是相同的
    loop.loop();

    //要是走到这里那就是循环结束了·
    // MutexLockGuard lock(mutex_);        //访问loop_需要进行加锁·
    loop_ = NULL;
}