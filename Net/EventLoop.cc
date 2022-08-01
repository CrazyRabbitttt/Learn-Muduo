#include "Learn-Muduo/Net/EventLoop.h"


//todo: log 

#include <assert.h>
#include <poll.h>

using namespace bing;


//防止线程创建多个EventLoop
thread_local EventLoop* t_LoopInThisThread = 0;

EventLoop::EventLoop() 
    :looping_(false), threadId_(currentThread::tid())
{
    if (t_LoopInThisThread) {       //不是0
        printf("WARN:另一个事件循环在本线程中!!!!!\n");
    } else {
        t_LoopInThisThread = this;
    }
}



EventLoop::~EventLoop() {
    assert(!looping_);
    t_LoopInThisThread = NULL;  //exit, then let loop to nullptr 
}


void EventLoop::loop() {
    //
    assert(!looping_);

    assertInLoopThread();       //判断下是不是
    looping_ = true;
    ::poll(NULL, 0, 5*1000);       //poll for 5 seconds

    printf("EventLoop stop looping..\n");
    looping_ = false;
}

void EventLoop::updateChannel(Channel* channel) {
    
}


void EventLoop::abortNotInLoopThread() {
    printf("WARN:abort not in this thread!!!!\n");
}

