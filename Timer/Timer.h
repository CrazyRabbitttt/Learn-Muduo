#ifndef BING_TIMER_H
#define BING_TIMER_H

#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/TimeStamp.h"

#include <functional>


namespace bing {

class Timer  : nocopyable{
 public:
    using BasicCallback = std::function<void()>;
    Timer(TimeStamp timer, BasicCallback&& callback, double interval);
    ~Timer() = default;

    void Run() {
        callback_();                    // run the function 
    }

    TimeStamp expiration() const {
        return expiration_;
    }

    void Restart(TimeStamp now) {
        expiration_ = TimeStamp::AddTime(now, interval_);
    }

    bool repeat() const {
        return repeat_;
    }

 private:
    BasicCallback callback_;
    TimeStamp expiration_;              // 超时事件戳
    double interval_;                   // tick时间间隔
    bool repeat_;           

};  




}   // namespace bing 

#endif