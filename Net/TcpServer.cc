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

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& nameArg)
    : loop_(checkLoopNotNull(loop)),
      name_(nameArg),
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

    //进行监听
    if (!accepter_->listening()) {
        //调用accpetcor的listen, get获得unique_ptr的普通的指针
        loop_->runInLoop(std::bind(&Accepter::listen, accepter_.get()));
    }
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    size_t n = connections_.erase(conn->name());      //从map中删除掉
    loop_->queueInLoop(std::bind(&TcpConnection::connectDestoryed, conn));
}



//新连接来了， 执行acceptor的回调函数
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {
    //创建TcpConnectionPtr
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    //创建localAddr
    sockaddr_in local;
    ::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;

    if (::getsockname(sockfd, (sockaddr*)& local, &addrlen) < 0) {
        printf("error : TcpServer::newConnection getlocaladdr error\n");
    }

    InetAddress localAddr(local);       //sockfd
    TcpConnectionPtr conn(new TcpConnection(
        loop_, connName, sockfd, localAddr, peerAddr
    ));

    //将连接加入到map中
    connections_[connName] = conn;

    conn->setConnectionCallBack(connectioncb_);
    conn->setMessageCallBack(messagecb_);
    conn->setCloseCallBack(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    conn->connectEstablished();     //调用connection的回调函数，建立连接

}


