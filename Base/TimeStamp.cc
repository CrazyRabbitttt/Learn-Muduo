#include "TimeStamp.h"

#include <time.h>

using namespace bing;

TimeStamp::TimeStamp() : microSecondsSinceEpoll_(0) {}


TimeStamp::TimeStamp(int64_t microSecondsSinceEpoll)
    :microSecondsSinceEpoll_(microSecondsSinceEpoll)
    {}

TimeStamp TimeStamp::now() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return TimeStamp(time.tv_sec * kMicroSecond2Second + time.tv_usec);         // 单位是微秒
}

TimeStamp TimeStamp::AddTime(const TimeStamp& timestamp, double add_seconds) {
    // 增加时间，单位是微秒
    int64_t addSeconds = static_cast<int64_t>(add_seconds) * kMicroSecond2Second;
    return TimeStamp(timestamp.microSecond() + addSeconds);
}


std::string TimeStamp::toString() const {
    char buf[128] = {0};
    time_t nowSeconds = static_cast<time_t>(microSecondsSinceEpoll_ / kMicroSecond2Second);
    struct tm time;
    localtime_r(&nowSeconds, &time);
    int64_t nowMicroSecond = microSecondsSinceEpoll_ % kMicroSecond2Second;
    snprintf(buf, sizeof buf, "%4d%02d%02d %02d:%02d:%02d.%06d",
        1900+time.tm_year, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, nowMicroSecond);
    return buf;
}




