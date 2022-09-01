#include "TimerQueue.h"
#include "EventLoop.h"
#include "Channel.h"

#include <sys/timerfd.h>
#include <assert.h>
using namespace bing;

TimerQueue::TimerQueue(EventLoop* loop) 
    :loop_(loop), timerfd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC)), channel_(new Channel(loop, timerfd_))
{   
    // InitChannel(loop, timerfd_);
    // 注册读事件
    channel_->setReadCallBack(std::bind(&TimerQueue::HandleRead, this));
    channel_->enableReading();
}    


TimerQueue::~TimerQueue() {
    // 关闭掉channel的
    channel_->disableAll();
    loop_->removeChannel(channel_.get());
    close(timerfd_);

    for (auto & it : timers_) {
        delete it.second;
    }
}

void TimerQueue::AddTimer(TimeStamp timestamp, BasicFunc&& cb, double interval) {
    // pass in : 超时时间戳，回调函数，时间间隔
    Timer* timer(new Timer(timestamp, std::move(cb), interval));
    loop_->runInLoop(std::bind(&TimerQueue::AddTimerInloop, this, timer));
}


void TimerQueue::AddTimerInloop(Timer* timer) {
    bool resetInstant = false;
    resetInstant = Insert(timer);
    if (resetInstant) {
        ResetTimer(timer);
    }
}

void TimerQueue::ResetTimer(Timer* timer) {
    // 重新设置timer的超时时间
    struct itimerspec newtime;
    struct itimerspec oldtime;
    memset(&oldtime, '\0', sizeof oldtime);
    memset(&newtime, '\0', sizeof newtime);

    // 看一下diff seconds
    int64_t seconds_diff = static_cast<int64_t>(timer->expiration().microSecond() - TimeStamp::now().microSecond());

    if (seconds_diff < 100) seconds_diff = 100;

    newtime.it_value.tv_sec  = static_cast<time_t>(seconds_diff / kMicroSecond2Second);                // 到期时间传入的参数是diff seconds?
    newtime.it_value.tv_nsec = static_cast<long>((seconds_diff % kMicroSecond2Second) * 1000);         // 毫秒的差距？
    int ret = ::timerfd_settime(timerfd_, 0, &newtime, &oldtime);
    assert(ret != -1);
    (void) ret;  
}

void TimerQueue::ResetTimers() {
    for (auto & it : activeTimers_) {
        if (it.second->repeat()) {
            auto timer = it.second;
            timer->Restart(TimeStamp::now());
            Insert(timer);
        } else {
            delete it.second;       
        }
    }

    if (!timers_.empty()) {
        // 超时时间就是最近的那个
        ResetTimer(timers_.begin()->second);  
    }

}


bool TimerQueue::Insert(Timer* timer) {
    bool reset_instantly = false;
    // 新插入的Timer是最先被触发的
    if (timers_.empty() || timer->expiration() < timers_.begin()->first) {
        reset_instantly = true;
    }

    // use move to avoid one copy assign
    timers_.emplace(std::move(TimerEntry(timer->expiration(), timer)));
    return reset_instantly;
}


void TimerQueue::ReadFromFd() {
    uint64_t byte_read;
    ssize_t n = ::read(timerfd_, &byte_read, sizeof byte_read);
    if (n != sizeof byte_read) {
        printf("TimerQueue::ReadFromFd read SYS_ERR\n");
    }
}


void TimerQueue::HandleRead() {
    ReadFromFd();
    
    // 下面将所有的超时的Timer进行处理
    TimeStamp expiration_time(TimeStamp::now());
    activeTimers_.clear();

    // end所指向的就是第一个没到期的Timer, 前面的都是到期了的
    auto end = timers_.lower_bound(TimerEntry(expiration_time, reinterpret_cast<Timer*>(UINTPTR_MAX)));
    // 将timers中到期了的加入到activeTimers中， (添加到vec中的开始位置，src开始位置，src末尾位置) 当然是不包含end的，[)
    activeTimers_.insert(activeTimers_.end(), timers_.begin(), end);                    

    // 将这些超时了的删除掉
    timers_.erase(timers_.begin(), end);  

    for (const auto& it : activeTimers_) {
        it.second->Run();
    }      

    ResetTimers();      // 就是因为忘了这一步？·
}

