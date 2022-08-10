#ifndef BING_NET_EVENTLOOPTHREAD_H
#define BING_NET_EVENTLOOPTHREAD_H

#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/Mutex.h"
#include "Learn-Muduo/Base/Condition.h"
#include "Learn-Muduo/Base/Thread.h"

namespace bing {

class EventLoop;            //forward declaration

class EventLoopThread : nocopyable{

 public:
    using ThreadInitCallbacl = std::function<void(EventLoop* )>;

   //  EventLoopThread(const ThreadInitCallbacl& cb = ThreadInitCallbacl());
   EventLoopThread();
    ~EventLoopThread();

    EventLoop* startLoop();

 private:

     void threadFunc();
    
     EventLoop* loop_;       //前置声明，用的必须是指针
     Thread thread_;
     bool exiting_;

     MutexLock mutex_;   
     Condition cond_;
   //   ThreadInitCallbacl callback_;
};

}   //namespace bing

#endif