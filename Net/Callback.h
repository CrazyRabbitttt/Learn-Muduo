#ifndef BING_NET_CALLBACK_H
#define BING_NET_CALLBACK_H

#include <memory>
#include <functional>

class TcpConnection;
class Buffer;
class Timestamp;    

using TimerCallback = std::function<void()>;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
// using MessageCallback = std::function<void(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime)>;
using MessageCallBack = std::function<void(const TcpConnectionPtr&, const char* data, ssize_t len)>;



#endif
