#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/Poller.h"

//todo: log 

#include <assert.h>
#include <poll.h>

using namespace bing;


//防止线程创建多个EventLoop
thread_local EventLoop* t_LoopInThisThread = 0;

const int kPollTimeMs = 10000;


EventLoop::EventLoop() 
    :looping_(false), threadId_(currentThread::tid()), quit_(false), poller_(new Poller(this))
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
    quit_ = false;
    //将事件进行调用了
    while (!quit_) {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);       //获得事件的活动列表
        for (ChannelList::iterator it = activeChannels_.begin(); 
            it != activeChannels_.end(); ++it) 
        {
            (*it)->handleEvent();
        }
    }
    printf("EventLoop stop looping..\n");
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
}


void EventLoop::updateChannel(Channel* channel) {
    poller_->updateChannel(channel);
}


void EventLoop::abortNotInLoopThread() {
    printf("WARN:abort not in this thread!!!!\n");
}

