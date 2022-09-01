#include "Timer.h"
using namespace bing;

Timer::Timer(TimeStamp expiration, BasicCallback&& callback, double interval = 0.0) 
    : expiration_(expiration), callback_(std::move(callback)), interval_(interval), repeat_(interval > 0.0) 
{}
