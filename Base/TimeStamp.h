#ifndef BING_BASE_TIMESTAMP_H
#define BING_BASE_TIMESTAMP_H

#include <iostream>

namespace bing {

class TimeStamp {
public:
    explicit TimeStamp(int64_t microSecondsSinceEpoll);

    TimeStamp();

    static TimeStamp now();

    std::string toString() const;


private:
    int64_t microSecondsSinceEpoll_;
};


}       //namespace bing
#endif