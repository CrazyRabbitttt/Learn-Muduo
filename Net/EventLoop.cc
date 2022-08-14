#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/Poller.h"

//todo: log 

#include <sys/eventfd.h>
#include <assert.h>
#include <poll.h>
#include <vector>

using namespace bing;


//防止线程创建多个EventLoop
thread_local EventLoop* t_LoopInThisThread = 0;

//默认的IO多路复用的超时时间
const int kPollTimeMs = 5000;


//创建wakeupfd_,唤醒subReactor处理新的Channel
int createEventfd() {
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        printf("create eventfd error:%d\n", errno);
    }
    return fd;
}


EventLoop::EventLoop() 
    :looping_(false), threadId_(currentThread::tid()), quit_(false), poller_(Poller::newDefaultPoller(this)) ,
        wakeupFd_(createEventfd()), 
        wakeupChannel_(new Channel(this, wakeupFd_)), 
        callingPengdingChannel_(false)
{
    if (t_LoopInThisThread) {       //不是0
        printf("WARN:另一个事件循环在本线程中!!!!!\n");
    } else {
        t_LoopInThisThread = this;
    }

    //wakeupChannel_需要注册读的事件, handleRead：进行读的处理  
    wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handleRead, this));
    //每个EventLoop监听wakeup的读事件
    wakeupChannel_->enableReading();
}


EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();   //不用唤醒了，关闭关注的所有的事件
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_LoopInThisThread = NULL;  //exit, then let loop to nullptr 
}


void EventLoop::loop() {

    assertInLoopThread();       //判断下是不是
    looping_ = true;
    quit_ = false;
    //将事件进行调用了
    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);       //获得事件的活动列表
        // printf("the num of channel: %d\n", activeChannels_.size());
        printf("the num of activechannels: %d\n", static_cast<int>(activeChannels_.size()));
        for (ChannelList::iterator it = activeChannels_.begin(); 
            it != activeChannels_.end(); ++it) 
        {
            (*it)->handleEvent(pollReturnTime_);
        }

        //上面是client的事件
        /**
         * IO线程(mainReactor)只接收新用户的连接，并将accept返回的fd封装到一个Channel中
         * 而已连接用户的Channel要分发给subloop
         * 所以mainloop要事先注册一个回调cb(需要subloop来执行)
         * mainloop通过eventfd唤醒subloop后，会执行上面的handleEvent，构造函数中将他注册为handleRead，也就是读一个8字节的数据，用来唤醒subloop
         * 然后执行下面的doPendingFunctors()，也就是mainloop事先注册的回调操作cb。
         */
        doPendingFunctors();
    }
    // printf("EventLoop stop looping..\n");
    looping_ = false;
}

/*
退出EventLoop, 
1如果是在自己的线程中调用quit，直接退出
2如果在其他线程调用quit，将该loop唤醒，才会结束
*/
void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

//当前Loop进行回调
void EventLoop::runInLoop(const Functor& cb) {
    if (isInLoopThread()) {     //当前线程loop,直接执行
        cb();   
    } else {
        queueInLoop(cb);          //放入队列中， 唤醒loop所在线程，执行callback
    }
}

//将callback函数放入队列中，唤醒线程
void EventLoop::queueInLoop(const Functor& cb) {
    {
        MutexLockGuard lock(mutex_);
        // pendingFunctors_.push_back(cb);
        pendingFunctors_.emplace_back(cb);
    }

    //唤醒loop线程
    //不在当前线程或者是当前线程正在执行回调
    if (!isInLoopThread() || callingPengdingChannel_) {
        wakeup();       
    }
}

//唤醒线程，需要往线程发送数据，致使可读事件被调用
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        printf("EventLoop::wakeup() error, write %lu bytes\n", n);
    }
}


//被可读事件唤醒的处理，读取数据，取消掉这个发送的一个int
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one))
    {
        printf("EventLoop::handleRead() reads %ld bytes intstead of 8 \n", n);
    }
}


//执行回调函数
void EventLoop::doPendingFunctors() {
    //将vector进行复制到局部变量，减少锁的粒度，快速执行
    std::vector<Functor> localFunctors;

    callingPengdingChannel_ = true;     //开始执行回调函数

    {
        MutexLockGuard lock(mutex_);
        //将函数拿到局部变量中并且原来的进行了置空操作
        localFunctors.swap(pendingFunctors_);       //直接拿到局部变量中
    }   

    for (const Functor& func : localFunctors) {
        func();     //进行调用函数
    }

    callingPengdingChannel_ = false;    //执行完毕
}


void EventLoop::updateChannel(Channel* channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) {
    return poller_->hasChannel(channel);
}


void EventLoop::abortNotInLoopThread() {
    printf("WARN:abort not in this thread!!!!\n");
}

