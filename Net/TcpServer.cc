#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/TcpConnection.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/Accepter.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

using namespace bing;


static EventLoop* checkLoopNotNull(EventLoop* loop) {
    if (loop == nullptr) {
        printf("%s:%s:%d mainLoop is null \n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : loop_(checkLoopNotNull(loop)),
      name_(listenAddr.toIpPort()),
      accepter_(new Accepter(loop, listenAddr)),
      started_(false),
      nextConnId_(1)
    {
        //新的用户连接的时候执行newConnection的回调
        accepter_->setNewConnectioncallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
    }  

TcpServer::~TcpServer() {
    //将所有的connection进行释放掉
}

void TcpServer::start() {
    if (!started_) { started_ = true; }

    if (!accepter_->listening()) {
        //调用accpetcor的listen
        loop_->runInLoop(std::bind(&Accepter::listen, accepter_.get()));
    }
}

//创建新的连接，就是执行的accept的回调， 接受完之后创建连接
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    const std::string connName = name_ + buf;
    
    //通过socket获得本机的ip & port 
    sockaddr_in local;
    ::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;
    if (::getsockname(sockfd, (sockaddr*)& local, &addrlen) < 0) {
        printf("error : TcpServer::newConnection getlocaladdr error\n");
    }
    InetAddress localAddr(local);

    //创建TcpConnectionPtr
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
    
    //ADD TO connections,(name, connectionptr)
    connections_[connName] = conn;


    

    conn->setConnectionCallBack(connectioncb_);
    conn->setMessageCallBack(messagecb_);
    conn->connectEstablished();                         //调用connection callback, 建立连接
}


