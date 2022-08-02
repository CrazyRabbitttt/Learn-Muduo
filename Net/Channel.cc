#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/EventLoop.h"

#include <sstream>
#include <poll.h>

using namespace bing;


const int Channel::kNoneEvent  = 0;
const int Channel::kReadEvent  = POLLIN | POLLPRI;      //读， 紧急读事件
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1)
    {}

void Channel::update() {
    loop_->updateChannel(this);
}

Channel::~Channel() {}


//根据revent去调用不同的用户回调
void Channel::handleEvent() {
    if (revents_ & POLLNVAL) {
        //todo: LOG_WARN
        printf("Channel::handleEvent, invalid event\n");
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorEventCallBack_) errorEventCallBack_();
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readEventCallBack_) readEventCallBack_();
    }

    if (revents_ & POLLOUT) {
        if (writeEventCallBack_) writeEventCallBack_();
    }
}



