#ifndef BING_LOG_LOGSTREAM_H
#define BING_LOG_LOGSTREAM_H

#include "Learn-Muduo/Base/nocopyable.h"
#include <algorithm>
#include <string.h>     // memcpy 
#include <string>

namespace bing {
    
static const int kSmallBuffer = 4000;          // 供LogStream使用
static const int kLargeBuffer = 4000 * 1000;   // 供AsyncLog适用
static const int kMaxNumericSize = 32;         // the max length of num 

template<int SIZE>      // 使用的时候指定int类型的大小 ： LogBuffer<kSmallBuffer>
class LogBuffer : public nocopyable {
 public:
    LogBuffer();
    ~LogBuffer();

    inline static void CookieStart();
    inline static void CookieEnd();

    void setCookie(void (*cookie)()) { cookie_ = cookie; }

    // 添加内容到缓冲区中
    void append(const char* buf, int len) {
        if (avial() >= len) {
            memcpy(cur_, buf, len);     // copy len bytes from buf to data(begin at cur_)
            cur_ += len;
        }
    }

    // 头指针
    const char* data() const { return data_; }

    // 已经使用的长度
    const int len() const { return static_cast<int>(cur_ - data_); }

    // 当前指针后移len个位置
    void add(size_t len)  { cur_ += len; }

    // 当前指针的位置
    char* current() { return cur_; }

    // 重置缓冲区
    void reset() { cur_ = data_; }

    void bzero() { memset(data_, 0, sizeof data_); }

    //  1 2 3 4 5 6 7 8 9    
    //  | | | | | | | | 
    // 缓冲区剩余空间的大小
    int avial() const { return static_cast<int>(end() - cur_); }

    // 缓冲区末尾的指针？
    const char* end() const { return data_ + sizeof(data_); }
 private:
    void (*cookie_)();      // cookie 用来记录函数的位置？
    char data_[SIZE];       // 缓冲区
    char* cur_;             // 当前位置的指针

};


/*
    日志类：使用stream类型的输出
*/


class LogStream {
 public:
    using self = LogStream;
    using Buffer = LogBuffer<kSmallBuffer>;     // 缓冲区比较小，主要是将一条日志缓存成为一行

    LogStream() {}
    ~LogStream() {}

    // LogStream have a buf to store the log, but is small
    const Buffer& buffer() { return buffer_; }

    template<typename T>
    size_t convert(char buf[], T value) {
        // 使用辗转相余倒转法
        T i = value;
        char *p = buf;               // point to the begin of the buf, used to index
        do {
            int a = i % 10;
            i /= 10;
            *p++ = a + '0';
        } while (i != 0);

        if (value < 0) *p++ = '-';
        *p = '\0';
        std::reverse(buf, p);       
        return p - buf;
    }


    // 将Interger类型(int, unsigned int, longlong)格式化到缓冲区中
    template<class T>
    void formatInterger(T num) {    
        if (buffer_.avial() >= kMaxNumericSize) {
            size_t len = convert(buffer_.current(), num);
            buffer_.add(len);   
        }
    }

    // 重载 << 运算符 (将内容写到缓冲区中), 前面的是基本的进行<<重载的类型，下面的调用基本重载
    self &operator<<(bool boolean) {
        // buffer_.append(boolean ? "1" : "0", 1);
        return (*this) << (boolean ? 1 : 0);    
    }

    self &operator<<(int num) {
        formatInterger(num);
        return *this;
    }


    self &operator<<(short num) {
        return (*this) << static_cast<int>(num);
    }
    self &operator<<(unsigned short num) {
        return (*this) << static_cast<int>(num);
    }
    self &operator<<(unsigned int num) {
        formatInterger(num);
        return *this;
    } 
    self &operator<<(long num) {
        formatInterger(num);
        return *this;
    }
    self &operator<<(unsigned long num) {
        formatInterger(num);
        return *this;
    }
    self &operator<<(long long num) {
        formatInterger(num);
        return *this;
    } 
    self &operator<<(unsigned long long num) {
        formatInterger(num);
        return *this;
    }
    self &operator<<(const void *data) {
        return (*this) << static_cast<const char*>(data);
    }

    self &operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }

    self &operator<<(double num) {
        char buf[32];
        int len = snprintf(buf, sizeof (buf), "%g", num);
        buffer_.append(buf, len);
        return *this;
    }
    self &operator<<(char v) {
        // 如果是字符的话，直接添加到缓冲区就好了
        buffer_.append(&v, 1);
        return *this;
    }
    self &operator<<(const char* str) {
        if (str) {          // sizeof 不能获得动态分配的存储空间的大小，这里用sizeof 的话就是指针的大小了 ： 8
            buffer_.append(str, static_cast<int>(strlen(str)));
            return *this;
        } else {
            buffer_.append("(null)", 6);
            return *this;
        }
    }
    self &operator<<(const unsigned char* v) {
        return operator<<(reinterpret_cast<const char*>(v));
    }
    self &operator<<(const std::string &v) {
        buffer_.append(v.c_str(), static_cast<int>(v.size()));
        return *this;
    }
    self &operator<<(const Buffer& v) {
        buffer_.append(v.data(), v.len());
        return *this;
    }

    void append(const char* data, int len) { buffer_.append(data, len); }

    //重置缓冲区
    void resetBuffer() { buffer_.reset(); }
 private:
    Buffer buffer_;                         // 缓冲区
};


}       //namespace bing 


#endif