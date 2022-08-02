#include "Learn-Muduo/Base/TimeStamp.h"

#include <time.h>

using namespace bing;

TimeStamp::TimeStamp() : microSecondsSinceEpoll_(0) {}


TimeStamp::TimeStamp(int64_t microSecondsSinceEpoll)
    :microSecondsSinceEpoll_(microSecondsSinceEpoll)
    {}

TimeStamp TimeStamp::now() {
    return TimeStamp(time(NULL));       //Return now 
}

std::string TimeStamp::toString() const {
    char buf[128] = {0};
    struct tm *tm_time = localtime(&microSecondsSinceEpoll_);
    snprintf(buf, sizeof (buf), "%4d%02d%02d %02d:%02d:%02d",
			 tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
			 tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
    return buf;
}




