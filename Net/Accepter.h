#ifndef BING_NET_ACCEPTER_H
#define BING_NET_ACCEPTER_H
#include "nocopyable.h"
#include "Channel.h"
#include "Socket.h"


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

    //接受到新用户连接之后执行回调函数
    void setNewConnectioncallback(const NewConnectionCallBack& cb) {
        newConnectioncb_ = cb;
    }
    
    bool listening() const { return listening_; }

    void listen();
private:        

    //进行listen到的处理，进行accept
    void NewConnection();  

    EventLoop* loop_;            //main loop, 用户定义的事件循环
    Channel acceptChannel_;     
    Socket  acceptSocket_;    
    NewConnectionCallBack newConnectioncb_;     //
    bool listening_;
};


}   //namespace bing

#endif