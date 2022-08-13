#include "Learn-Muduo/Net/TcpConnection.h"
#include "Learn-Muduo/Net/Socket.h"
#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/EventLoop.h"

using namespace bing;


static EventLoop* CheakLoopNotNull(EventLoop* loop) {
    if (loop == nullptr) {
        printf("%s:%s:%d mainloop is null\n", __FILE__, __FUNCTION__, __LINE__ );
    }
    return loop;
}



TcpConnection::TcpConnection(EventLoop* loop,
                            const std::string nameArg, 
                            int sockfd, 
                            InetAddress localAddr,
                            InetAddress peerAddr)
    : loop_(CheakLoopNotNull(loop)),
      name_(nameArg),
      state_(kConnecting),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop, sockfd)),
      localaddr_(localAddr),
      peeraddr_(peerAddr)
    {
        //进行可读事件的回调
        channel_->setReadCallBack(std::bind(&TcpConnection::handleRead, this));
    }

TcpConnection::~TcpConnection() {
    printf("~TcpConnection, fd = %d\n", channel_->fd());
}


void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setStata(kConnected);
    channel_->enableReading();      //注册可读， socket

    connectioncb_(shared_from_this());
}


void TcpConnection::handleRead() {
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof (buf));
    messagecb_(shared_from_this(), buf, n);                     //Message Call Back
}
