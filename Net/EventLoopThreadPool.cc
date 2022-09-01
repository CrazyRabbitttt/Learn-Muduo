#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

using namespace bing;

// server调用，会有一个baseLoop_
EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop)
    :baseLoop_(baseloop), started_(false), numThreads_(0), next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool() {
    // eventloop都是栈上面的对象，不用在析构函数中进行delete
}


// 开启事件循环， 
void EventLoopThreadPool::start() {
    started_ = true;
    // 创建线程
    for (int i = 0; i < numThreads_; i++) {
        // 线程命名
        char buf[64];
        snprintf(buf, sizeof buf, "%s%d", "线程池:", i);
        // 创建EventLoopThread
        EventLoopThread *t = new EventLoopThread();
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));    // 放入threads_

        /*
            EventLoopThread会创建子线程，子线程创建EventLoop, 放入loops中
        */
       loops_.push_back(t->startLoop());
    }

    // 如果说服务端只是有一个线程， 运行baseLoop
    if (numThreads_ == 0) {
        // do nothing 
    }
}


// 轮询的方式分配Channel给subloop

EventLoop* EventLoopThreadPool::getNextLoop() {
    EventLoop* loop = baseLoop_;

    if (!loops_.empty()) {
        loop = loops_[next_];
        ++next_;
        if (next_ >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}






