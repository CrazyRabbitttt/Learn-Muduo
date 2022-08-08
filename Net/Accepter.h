#ifndef BING_NET_ACCEPTER_H
#define BING_NET_ACCEPTER_H
#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/Socket.h"


namespace bing {

class EventLoop;
class InetAddress;

/*
    监听， listenfd, 如果有新的用户请求连接
    那么就进行处理
*/
class Accepter : nocopyable{
public:
    using NewConnectionCallBack = std::function<void(int sockfd, const InetAddress)>;
    
    Accepter(EventLoop* loop, const InetAddress& listenAddr, bool reuseport = true);

    ~Accepter();

    void setNewConnectioncallback(const NewConnectionCallBack& cb) {
        newConnectioncb_ = cb;
    }
    
    bool listening() const { return listening_; }

    void listen();
private:        

    //进行listen到的处理，进行accept
    void handleRead();  

    EventLoop* loop_;            //main loop
    Channel acceptChannel_;     
    Socket  acceptSocket_;    
    NewConnectionCallBack newConnectioncb_;     //
    bool listening_;
};


}   //namespace bing

#endif