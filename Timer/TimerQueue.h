#ifndef BING_TIMER_TIMERQUEUE_H_
#define BING_TIMER_TIMERQUEUE_H_

#include "Learn-Muduo/Timer/Timer.h"
#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/TimeStamp.h"
#include "Learn-Muduo/Net/EventLoop.h"

#include <vector>
#include <set>

namespace bing {

class TimerQeuue : nocopyable{



 private:   
    using TimerEntry = std::pair<TimeStamp, Timer>;
    using TimerSet    = std::set<TimerEntry>;
    using ActiveTimers = std::vector<TimerEntry>;

    // 怎么去应对，多个定时器超时时间相同？

    EventLoop* loop_;
    int timerfd_;
    std::unique_ptr<Channel> channel_;

    TimerSet timers_;
    ActiveTimers activeTimers_;

};


}   // namespace bing

#endif 