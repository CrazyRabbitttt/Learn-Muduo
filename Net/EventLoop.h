#ifndef BING_NET_EVENTLOOP_H
#define BING_NET_EVENTLOOP_H

#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/CurrentThread.h"

namespace bing {


class EventLoop : nocopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() { return threadId_ == bing::currentThread::tid(); }


private: 

    void abortNotInLoopThread();        //警告不是在本线程中·

    bool looping_;
    const pid_t threadId_;          //线程标识

};






}  //namespace bing 

#endif