#ifndef BING_TIMER_TIMERQUEUE_H_
#define BING_TIMER_TIMERQUEUE_H_

#include "Learn-Muduo/Timer/Timer.h"
#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/TimeStamp.h"

#include <vector>
#include <set>
#include <string.h>
#include <functional>
#include <memory>

namespace bing {

class EventLoop;
class Channel;

class TimerQueue : nocopyable{
 public:    
    using BasicFunc = std::function<void()>;
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    void AddTimer(TimeStamp timestamp, BasicFunc&& cb, double interval);

 private:  
    using TimerEntry   = std::pair<TimeStamp, Timer*>;
    using TimerSet     = std::set<TimerEntry>;
    using ActiveTimers = std::vector<TimerEntry>;

    void HandleRead();

    void ReadFromFd();

    void AddTimerInloop(Timer* timer);
    // Return true if Reset instanly
    bool Insert(Timer* timer);

    void ResetTimer(Timer* timer);

    void ResetTimers();

    // 怎么去应对，多个定时器超时时间相同？

    EventLoop* loop_;
    int timerfd_;
    std::unique_ptr<Channel> channel_;

    TimerSet timers_;
    ActiveTimers activeTimers_;

};


}   // namespace bing

#endif 