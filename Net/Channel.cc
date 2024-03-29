#include "Channel.h"
#include "EventLoop.h"

#include <sstream>
#include <sys/epoll.h>

using namespace bing;


const int Channel::kNoneEvent  = 0;
const int Channel::kReadEvent  = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel() {}

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false)
    {}

void Channel::update() {
    loop_->updateChannel(this);
}

// TcpConnection 底层管理了Channel, 执行回调的时候被销毁了怎么办？
// 使用tie试探tcpconnection是否是存活的状态
void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_  = obj;         // 用weak_ptr接收Connection对象
    tied_ = true;
}


void Channel::Init(EventLoop* loop, int fd) {
    this->loop_ = loop;
    this->fd_ = fd;
    this->events_ = 0;
    this->revents_ = 0;
    this->index_ = -1;
}

Channel::~Channel() {}

//在所属的EventLoop中将这个Channel删除掉
void Channel::remove() {
    loop_->removeChannel(this);
}




//根据revent去调用不同的用户回调, 得到Epoller的通知进行处理
void Channel::handleEvent(TimeStamp receiveTime) {      // 得到的是poll的return time, epoll 不断的
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeEventCallBack_) {
            closeEventCallBack_();
        }
    }

    //读事件
    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readEventCallBack_) {
            readEventCallBack_(receiveTime);
        }
    }
    
    //写事件
    if (revents_ & EPOLLOUT) {
        if (writeEventCallBack_) {
            writeEventCallBack_();
        }
    }
}



