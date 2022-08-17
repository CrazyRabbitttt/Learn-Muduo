#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/TcpConnection.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/EventLoopThreadPool.h"
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
      threadPool_(new EventLoopThreadPool(loop)),
      started_(false),
      nextConnId_(1)
    {
        //新的用户连接的时候执行newConnection的回调
        accepter_->setNewConnectioncallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
    }  

TcpServer::~TcpServer() {
    //将所有的connection进行释放掉
}


void TcpServer::setThreadNum(int numThreads) {
    assert(numThreads >= 0);
    threadPool_->setThreaadNum(numThreads);
}

void TcpServer::start() {           // 启动线程池
    if (!started_) { started_ = true; threadPool_->start(); }

    // 唤醒subloop之后进行监听， 这个loop的作用就是进行创建连接
    if (!accepter_->listening()) {
        //调用accpetcor的listen, get获得unique_ptr的普通的指针
        loop_->runInLoop(std::bind(&Accepter::listen, accepter_.get()));
    }
}
// 将connection从connections_中移除
void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
    printf("TcpServer::removeConnectionInLoop [%s] - connection %s \n",
             name_.c_str(), conn->name().c_str());
    connections_.erase(conn->name());      //从map中删除掉
    // 获得conn所在的loop
    EventLoop* ioLoop_ =  conn->getLoop();
    ioLoop_->queueInLoop(std::bind(&TcpConnection::connectDestoryed, conn));
}



//新连接来了， 执行acceptor的回调函数
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {

    // 轮询算法选择一个subLoop来进行Channel的管理
    EventLoop* ioLoop = threadPool_->getNextLoop();

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
        ioLoop, connName, sockfd, localAddr, peerAddr       // 这里的loop传入的是轮询得到的EventLoop
    ));

    //将连接加入到map中
    connections_[connName] = conn;
    // 用户设置的回调函数
    conn->setConnectionCallBack(connectioncb_);
    conn->setMessageCallBack(messagecb_);
    conn->setWriteCompeleteCallBack(writecompletecb_);

    conn->setCloseCallBack(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    // 直接调用回调函数，建立连接
    conn->connectEstablished();    
    // 直接调用连接的建立
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));

}


