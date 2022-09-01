#include "logstream.h"

using namespace bing;

template<int SIZE>
void LogBuffer<SIZE>::CookieStart() {}

template<int SIZE>
void LogBuffer<SIZE>::CookieEnd() {}

template<int SIZE>
LogBuffer<SIZE>::LogBuffer() : cur_(data_) {
    setCookie(CookieStart);
}

template<int SIZE>
LogBuffer<SIZE>::~LogBuffer() {
    setCookie(CookieEnd);
}


template class LogBuffer<kLargeBuffer>;
template class LogBuffer<kSmallBuffer>;