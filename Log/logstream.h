#pragma once

#include "Learn-Muduo/Base/nocopyable.h"

#include <string.h>     // memcpy 
#include <string>

const int kSmallBuffer = 4000;          // 供LogStream使用
const int kLargeBuffer = 4000 * 1000;   // 供AsyncLog适用
using namespace bing;

template<int SIZE>      // 使用的时候指定int类型的大小 ： LogBuffer<kSmallBuffer>
class LogBuffer : nocopyable {

 public:
    LogBuffer() : cur_(data_) {}
    ~LogBuffer() {}

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
    void add(size_t len) const { cur_ += len; }

    // 当前指针的位置
    char* current() { return cur_; }

    // 重置缓冲区
    void reset() { cur_ = data_; }

    //  1 2 3 4 5 6 7 8 9    
    //  | | | | | | | | 
    // 缓冲区剩余空间的大小
    int avial() const { return static_cast<int>(end() - cur_); }

 private:
    // 缓冲区末尾的指针？
    const char* end() const { return data_ + sizeof(data_); }

    char data_[SIZE];       // 缓冲区
    char* cur_;             // 当前位置的指针

};


/*
    日志类：使用stream类型的输出
*/


class LogStream : nocopyable{
    using self = LogStream;
 public:
    using Buffer = LogBuffer<kSmallBuffer>;


    // 重载 << 运算符 (将内容写到缓冲区中)
    self &operator<<(bool v) {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    self &operator<<(short);
    self &operator<<(unsigned short);
    self &operator<<(int);
    self &operator<<(unsigned int);
    self &operator<<(long);
    self &operator<<(unsigned long);
    self &operator<<(long long);
    self &operator<<(unsigned long long);
    self &operator<<(const void *);

    self &operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }

    self &operator<<(double);
    self &operator<<(char v) {
        // 如果是字符的话，直接添加到缓冲区就好了
        buffer_.append(&v, 1);
        return *this;
    }
    self &operator<<(const char* str) {
        if (str) {          // sizeof 不能获得动态分配的存储空间的大小，这里用sizeof 的话就是指针的大小了 ： 8
            buffer_.append(str, static_cast<int>(strlen(str)));
        } else {
            buffer_.append("(null)", 6);
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

    // 返回缓冲区
    const Buffer& buffer() { return buffer_; }

    //重置缓冲区
    void resetBuffer() { buffer_.reset(); }



 private:

    // 将Interger类型的格式化成为T类型的到缓冲区中
    template<class T>
    void formatInterger(T);

    Buffer buffer_;                         // 缓冲区
    static const int kMaxNumericSize = 32;
};


