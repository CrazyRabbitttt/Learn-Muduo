#include "Learn-Muduo/Log/logstream.h"

#include <stdint.h>                 //size-t
#include <algorithm>

using namespace bing;

const char digits[] = "9876543210123456789";   // 数字
const char* zero = digits + 9;                 // 零的位置
const char digitsHex[] = "0123456789ABCDEF";   // 十六进制

// 将Int类型转换为String类型的高效的函数
template<class T>
size_t convert(char buf[], T value) {
    T i = value;
    char* p = buf;
    // 进行处理，最后处理负数，然后进行reverse
    do {
        int a = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[a];                         // 将个位对应的字符写上去
    } while(i != 0)
    
    if (value < 0) {
        *p++ = '-';
    }
    *p = '\0';                                  // 填上末尾\0

    std::reverse(buf, p);
    return p - buf;                             // 返回字符串的长度

}


// 将Int类型的转换成为十六进制的字符串类型
size_t convertHex(char buf[], uintptr_t value) {
    uintptr_t i = value;
    char* p = buf;

    do {
        int a = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[a];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}



// 将整型按照T类型格式化到缓冲区中, 都是整形(int, unsigned int)
template <class T>
void LogStream::formatInterger(T v) {
    if (buffer_.avial() >= kMaxNumericSize) {
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}


// 进行 << 运算符的重载

/*
    将short转换成为int，使用下面的重载函数
*/

LogStream &LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned short v) {
    *this << static_cast<unsigned short>(v);
    return *this;
}

/*
    这部分全部都是各种类型的整形，使用formatinterger, 写到buffer种
*/


LogStream &LogStream::operator<<(int v) {
    formatInterger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned int v) {
    formatInterger(v);
    return *this;
}
LogStream &LogStream::operator<<(long v) {
    formatInterger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long v) {
    formatInterger(v);
    return *this;
}
LogStream &LogStream::operator<<(long long v) {
    formatInterger(v);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long  long v) {
    formatInterger(v);
    return *this;
}

LogStream &LogStream::operator<<(const void* p) {
    // 如果是指针的情况，转换成为16进制？？
    // 如果是指针，就转为16进制的形式
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if (buffer_.avial() > kMaxNumericSize)
    {
        char *buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, v);
        buffer_.add(len + 2);
    }
    return *this;
}

// doble 类型
LogStream &LogStream::operator<<(double v) {
    if (buffer_.avial() > kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}
