#ifndef BING_NET_CALLBACK_H
#define BING_NET_CALLBACK_H

#include <memory>
#include <functional>
#include "Learn-Muduo/Base/TimeStamp.h"     // 直接使用的timestamp对象， 引入头文件


namespace bing {

class TcpConnection;
class Buffer;



using TimerCallback = std::function<void()>;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp receiveTime)>;
// using MessageCallback = std::function<void(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime)>;
// using MessageCallBack = std::function<void(const TcpConnectionPtr&, const char* data, ssize_t len)>;

}

#endif
