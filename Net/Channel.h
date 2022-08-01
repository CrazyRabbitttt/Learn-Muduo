#ifndef BING_NET_CHANNEL_H
#define BING_NET_CHANNEL_H

#include <functional>
#include <memory>

#include "Learn-Muduo/Base/nocopyable.h"
//todo : timestamp.h


namespace bing {

class EventLoop;        //前向声明， 只能使用指针对象. 源文件再包含头文件

/*
    Channel class does not own the file descriptor
    fd : socket, eventfd, timerfd, signalfd 

    只负责一个文件描述符号的IO事件分发
    Channel将不同的IO事件进行回调
*/

class Channel : nocopyable {
public:
    using EventCallBack = std::function<void()>;
    //todo:readEventCallBack
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();         //进行事件的处理

    void setReadCallBack(const EventCallBack& cb) { readEventCallBack_  = std::move(cb); }
    void setWriteCallBack(const EventCallBack&cb) { writeEventCallBack_ = std::move(cb); }
    void setErrorCallBack(const EventCallBack&cb) { errorEventCallBack_ = std::move(cb); }
    
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }     //进行当前事件的设定

    //注册读事件, 监听读的事件
    void enableReading() { events_ |= kReadEvent; update(); }

    //for poller
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }


    EventLoop* ownerLoop() { return loop_; }


private:

    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;        //属于哪一个EventLoop事件循环
    const int fd_;          //Poller 监听的对象
    int events_;            //fd感兴趣的事件， bit pattern 
    int revents_;           //当前的事件
    int index_;             //used by poller


    //事件的回调，不同类型的回调函数
    EventCallBack writeEventCallBack_;
    EventCallBack readEventCallBack_;
    EventCallBack errorEventCallBack_;


};



} //namespace bing 


#endif