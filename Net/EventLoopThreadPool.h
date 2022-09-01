#ifndef BING_NET_EVENTLOOPTHREADPOOL_H
#define BING_NET_EVENTLOOPTHREADPOOL_H

#include "nocopyable.h"
#include <string>
#include <memory>
#include <vector>

namespace bing {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : nocopyable {
 public:
    EventLoopThreadPool(EventLoop* baseloop);
    ~EventLoopThreadPool();

    void setThreaadNum(int num) {numThreads_ = num; }

    // 开启事件循环线程
    void start();

    // 获得下一个需要操作的事件循环
    EventLoop* getNextLoop();


    bool started() { return started_; }


 private:
    EventLoop* baseLoop_;                                   // 如果没有设置线程的数目，那么就是单线程, tcpserver对应的loop, baseloop_
    // std::string name_;                                      // 线程池的名称
    bool started_;                                          // 是否开启
    int numThreads_;                                        // 线程的数目
    int next_;                                              // loops中的下一个要被使用的EventLoop
    std::vector<std::unique_ptr<EventLoopThread>> threads_; // 包含了所有的EventLoopThread
    std::vector<EventLoop* > loops_;                        // 所有线程创建的EventLoop
};

}   // namespace bing 

#endif