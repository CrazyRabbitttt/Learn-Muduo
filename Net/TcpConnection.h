#ifndef BING_NET_TCPCONNECTION_H
#define BING_NET_TCPCONNECTION_H

#include "Learn-Muduo/Net/InetAddress.h"
#include "Learn-Muduo/Net/Callback.h"
#include "Learn-Muduo/Base/nocopyable.h"
#include <memory>
#include <atomic>

namespace bing {

class EventLoop;
class Channel;
class Socket;

//class 用智能指针进行封装，用于返回this 
class TcpConnection : nocopyable, public std::enable_shared_from_this<TcpConnection>        
{
public: 
    TcpConnection(EventLoop* loop,
     const std::string name, 
     int sockfd, 
     InetAddress& localAddr,
     InetAddress& peerAddr);

    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return localaddr_; }
    const InetAddress& peerAddress() const { return peeraddr_; }
    bool connected() const { return state_ == kConnected; }

    void setConnectionCallBack(const ConnectionCallback& cb) {
        connectioncb_ = cb;
    }

    void setMessageCallBack(const MessageCallBack& cb) {
        messagecb_ = cb;
    }

    //进行连接的建立
    void connectEstablished();

private:
    enum State{ kConnecting, kConnected, };

    void handleRead();                          //将可读事件传递给客户， MessageCallBack
    void setStata(State state) { state_ = state; }
    EventLoop* loop_;                            //sub loop, 进行TcpConnection的处理

    std::string name_;
    InetAddress localaddr_;                     //本地地址，维护的是connection嘛
    InetAddress peeraddr_;                      //对端地址
    std::unique_ptr<Socket> socket_;            //进行连接的socket的维护
    std::unique_ptr<Channel> channel_;

    std::atomic_int state_;                     //state 
    ConnectionCallback connectioncb_;
    MessageCallBack messagecb_;

};

}   //namespace bing 

#endif 