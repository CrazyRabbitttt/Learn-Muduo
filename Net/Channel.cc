#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/EventLoop.h"

#include <sstream>
#include <sys/epoll.h>

using namespace bing;


const int Channel::kNoneEvent  = 0;
const int Channel::kReadEvent  = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;


Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1)
    {}

void Channel::update() {
    loop_->updateChannel(this);
}

Channel::~Channel() {}

//在所属的EventLoop中将这个Channel删除掉
void Channel::remove() {
    loop_->removeChannel(this);
}


//根据revent去调用不同的用户回调, 得到Epoller的通知进行处理
void Channel::handleEvent() {
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallBack_) {
            closeCallBack_();
        }
    }

    //读事件
    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readEventCallBack_) {
            readEventCallBack_();
        }
    }
    
    //写事件
    if (revents_ & EPOLLOUT) {
        if (writeEventCallBack_) {
            writeEventCallBack_();
        }
    }
}



