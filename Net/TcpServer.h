#ifndef BING_NET_TCPSERVER_H
#define BING_NET_TCPSERVER_H
#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Net/Callback.h"
#include "Learn-Muduo/Net/InetAddress.h"

#include <unordered_map>
#include <atomic>
namespace bing {

class EventLoop;
class Accepter;


class TcpServer : nocopyable {
 public:
    
    TcpServer(EventLoop* loop, const InetAddress& listenaddr);
    ~TcpServer();

    void setMessageCallback(const MessageCallBack& cb) { messagecb_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb) { connectioncb_ = cb; }
    // void setWriteComCallback(const WriteCompleteCallback& cb) { writecompletecb_ = cb; }


    void start();           //进行服务端的监听

 private:

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;    //name -> tcpconnectionPtr

    //创建新的连接,由Accepter进行回调
    void newConnection(int sockfd, const InetAddress& peeraddr);
    
    //连接断开了， 需要从connections_中移除掉
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;       //the main loop
    const std::string name_;
    
    std::unique_ptr<Accepter> accepter_;        //mainloop, 进行连接事件的监听

    ConnectionCallback connectioncb_;           //新连接的回调函数
    MessageCallBack messagecb_;                 //读写消息的回调
    // WriteCompleteCallback writecompletecb_;     //写成功的回调
    std::atomic_int started_;
    int nextConnfd_;
    ConnectionMap connections_;

};

}   //namespace bing 
#endif 