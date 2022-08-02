#ifndef BING_NET_EVENTLOOP_H
#define BING_NET_EVENTLOOP_H

#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/CurrentThread.h"

#include <vector>

namespace bing {

class Channel;          //前向声明
class Poller;

class EventLoop : nocopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    //会进行事件的注册，向poller传递
    void updateChannel(Channel* channel);

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() { return threadId_ == bing::currentThread::tid(); }


private: 
    using ChannelList = std::vector<Channel*>;
    void abortNotInLoopThread();        //警告不是在本线程中·

    bool looping_;
    bool quit_;

    std::unique_ptr<Poller> poller_;        //one loop, one poller 
    ChannelList activeChannels_;
    const pid_t threadId_;          //线程标识

};


}  //namespace bing 

#endif