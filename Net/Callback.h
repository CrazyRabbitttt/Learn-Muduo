#ifndef BING_NET_CALLBACK_H
#define BING_NET_CALLBACK_H

#include <memory>
#include <functional>
// #include "Learn-Muduo/Base/TimeStamp.h"     // 直接使用的timestamp对象， 引入头文件


namespace bing {

class TcpConnection;
class Buffer;
class TimeStamp;


using TimerCallback = std::function<void()>;


using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp receiveTime)>;

// 高水位的处理: 应用发送数据比较快，内核发送的满，堆积在缓冲区中了
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr&, size_t)>;

}

#endif
