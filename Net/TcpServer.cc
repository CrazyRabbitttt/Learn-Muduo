#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/Accepter.h"

#include <stdio.h>
#include<errno.h>
using namespace bing;


static EventLoop* checkLoopNotNull(EventLoop* loop) {
    if (loop == nullptr) {
        printf("%s:%s:%d mainLoop is null \n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}


TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenaddr)
    :loop_(checkLoopNotNull(loop)),
    name_(listenaddr.toIpPort()),
    accepter_(new Accepter(loop, listenaddr)),
    started_(false),
    nextConnfd_(1)
    {
        accepter_->setNewConnectioncallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
    }

TcpServer::~TcpServer() {}


void TcpServer::start() {
    //started_是源自的
    
}

