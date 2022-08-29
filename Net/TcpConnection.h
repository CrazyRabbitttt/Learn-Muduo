#ifndef BING_NET_TCPCONNECTION_H
#define BING_NET_TCPCONNECTION_H

#include "Learn-Muduo/Net/InetAddress.h"
#include "Learn-Muduo/Net/Callback.h"
#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Net/Buffer.h"
#include "Learn-Muduo/Http/httpContent.h"
#include "Learn-Muduo/Base/TimeStamp.h"
#include <memory>
#include <atomic>

namespace bing {

class EventLoop;
class Channel;
class Socket;

//class 用智能指针进行封装，用于返回this ,唯一进行了enable_shared_from_this 的类（生命期是很模糊的）
class TcpConnection : nocopyable, public std::enable_shared_from_this<TcpConnection>        
{
public: 

    // bing::EventLoop *, const std::__cxx11::string, int, bing::InetAddress, const bing::InetAddress

    TcpConnection(EventLoop* loop,
     const std::string name, 
     int sockfd, 
     const InetAddress& localAddr,
     const InetAddress& peerAddr);

    ~TcpConnection();

    //return reference by const 
    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return localaddr_; }
    const InetAddress& peerAddress() const { return peeraddr_; }
    bool connected() const { return state_ == kConnected; }

    // 发送数据
    void send(const std::string& buf);

    void send(Buffer* buffer);

    // 给到Server去填充
    HttpContent* getHttpContent() {
        return &content_;
    }

    // 关闭连接
    void shutdown();
    bool IsShutdown() const { return state_ == kDisConnecting; }
    // 关闭读端，用于HTTP短连接
    void shutdownRead();

    // 设置回调函数
    void setConnectionCallBack(const ConnectionCallback& cb) { connectioncb_ = cb; }
    void setMessageCallBack(const MessageCallback& cb) { messagecb_ = cb; }
    void setCloseCallBack(const CloseCallback& cb) { closecb_ = cb; } 
    void setWriteCompeleteCallBack(const WriteCompleteCallback& cb) { writecompeletecb_ = cb; }
    void setHighWaterMarkCallBack(const HighWaterMarkCallback& cb, size_t highWaterMark) {
        highWaterMarkcb_ = cb;
        highWaterMark_ = highWaterMark;
    }


    // 进行连接的建立
    void connectEstablished();                      //只能被调用一次
    // 进行连接的销毁
    void connectDestoryed();

    TimeStamp timestamp() const { return timestamp_; }

    void updateTimeStamp(TimeStamp now) { timestamp_ = now; }
private:
    enum State{ kConnecting, kConnected, kdisConnected, kDisConnecting, };

    void handleRead(TimeStamp receiveTime);                              // 将可读事件传递给客户， MessageCallBack
    void handleWrite();
    void handleError();
    void handleClose();


    void sendInloop(const void* data, size_t len);
    void shutdownInloop();


    void setState(State state) { state_ = state; }
    EventLoop* loop_;                               // sub loop, 进行TcpConnection的处理

    std::string name_;                              // 连接的名字
    InetAddress localaddr_;                         // 本地地址，维护的是connection嘛
    InetAddress peeraddr_;                          // 对端地址
    std::unique_ptr<Socket> socket_;                // 进行连接的socket的维护
    std::unique_ptr<Channel> channel_;

    std::atomic_int state_;                         // 连接的状态
    ConnectionCallback connectioncb_;
    MessageCallback messagecb_;
    CloseCallback closecb_;    
    WriteCompleteCallback writecompeletecb_;     
    HighWaterMarkCallback highWaterMarkcb_; 

    TimeStamp timestamp_;                           // 最新一次传递消息的时间戳
    size_t highWaterMark_;                          // 水位线

    HttpContent content_;                           // 保存了对端的报文数据， 内部的话同样保存了请求和响应报文
    Buffer inputBuffer_;                            // 接受数据的缓冲区，内部从tcp读完写到这里
    Buffer outputBuffer_;                           // 发送数据的缓冲区, 写到这里然后内部发送
};

}   //namespace bing 

#endif 