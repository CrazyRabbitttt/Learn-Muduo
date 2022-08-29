#ifndef BING_BASE_TIMESTAMP_H
#define BING_BASE_TIMESTAMP_H

#include <iostream>
#include <sys/time.h>

namespace bing {

const int kMicroSecond2Second = 1000 * 1000;

class TimeStamp {
public:
    explicit TimeStamp(int64_t microSecondsSinceEpoll);

    TimeStamp();

    static TimeStamp now();                 // will be directly used by extern file 

    static TimeStamp AddTime(const TimeStamp& timestamp, double add_seconds);

    std::string toString() const;

    int64_t microSecond() const { return microSecondsSinceEpoll_; }

    bool operator<(const TimeStamp& timestamp) const  {
        return microSecondsSinceEpoll_ < timestamp.microSecond();
    }

    bool operator==(const TimeStamp& timestamp) const {
        return microSecondsSinceEpoll_ == timestamp.microSecond();
    }

private:
    int64_t microSecondsSinceEpoll_;
};



}       //namespace bing
#endif