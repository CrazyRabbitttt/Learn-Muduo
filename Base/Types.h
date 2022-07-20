#ifndef _MUDUO_BASE_TYPES_H_
#define _MUDUO_BASE_TYPES_H_

#include <stdint.h>
#include <string.h>
#include <string>
#include <assert.h>
#include <string.h>

/*
    封装了一些常用的转换，更加简洁
*/

namespace Muduo {

using std::string;

inline void MemZero(void* p, size_t n) {
    memset(p, 0, n);
}


//进行隐式转换
template<typename To, typename From>
inline To implicit_cast(From const& f) {
    return f;
}

template<typename To, typename From>
inline To down_cast(From* f) {
    return static_cast<To> (f);
}




}




#endif
