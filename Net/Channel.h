#ifndef BING_NET_CHANNEL_H
#define BING_NET_CHANNEL_H

#include <functional>
#include <memory>

#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/TimeStamp.h"


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
    using ReadEventCallback = std::function<void(TimeStamp)>;

    Channel();
    Channel(EventLoop* loop, int fd);
    void Init(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(TimeStamp receiveTime);         //进行事件的处理

    void setReadCallBack(ReadEventCallback cb) { readEventCallBack_  = std::move(cb); }
    void setWriteCallBack(EventCallBack cb) { writeEventCallBack_ = std::move(cb); }
    void setErrorCallBack(EventCallBack cb) { errorEventCallBack_ = std::move(cb); }
    void setCloseCallBack(EventCallBack cb) { closeEventCallBack_ = std::move(cb); }

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }     //进行当前事件的设定

    //对于关注的事件的处理
    void enableReading()  { events_ |= kReadEvent; update();  }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void disableAll()     { events_ = kNoneEvent; update();   }
    void disableWriting() { events_ &= ~kWriteEvent; update();}
    void enableWriting()  { events_ |= kWriteEvent; update(); }


    //for Epoller, 目前的状态： added, new,
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    //fd当前的事件状态
    bool isNonEvent() const { return events_ == kNoneEvent; }       
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    EventLoop* ownerLoop() { return loop_; }

    void remove();

private:

    void update();          //将Channel绑定的文件描述符同EventLoop中的poll关联

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;        //属于哪一个EventLoop事件循环, 
    int fd_;                //Poller 监听的对象
    int events_;            //fd感兴趣的事件， bit pattern 
    int revents_;           //当前的事件
    int index_;             //used by poller， Epoller中的状态

    //事件的回调，不同类型的回调函数
    EventCallBack writeEventCallBack_;
    ReadEventCallback readEventCallBack_;
    EventCallBack errorEventCallBack_;
    EventCallBack closeEventCallBack_;

};



} //namespace bing 

#endif