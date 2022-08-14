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
                            const InetAddress& localAddr,
                            const InetAddress& peerAddr)
    : loop_(CheakLoopNotNull(loop)),
      name_(nameArg),
      state_(kConnecting),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop, sockfd)),
      localaddr_(localAddr),
      peeraddr_(peerAddr)
    {
        //进行可读事件的回调， socket上的可读事件被触发了
        channel_->setReadCallBack(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));      //绑定，传送参数
        channel_->setWriteCallBack(std::bind(&TcpConnection::handleClose, this));
        channel_->setErrorCallBack(std::bind(&TcpConnection::handleError, this));
        channel_->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));
    }

TcpConnection::~TcpConnection() {
    printf("~TcpConnection, fd = %d\n", channel_->fd());
}


void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setStata(kConnected);
    channel_->enableReading();      //注册可读， socket
    //调用callback 
    connectioncb_(shared_from_this());
}

//进行连接的销毁
void TcpConnection::connectDestoryed() {
    if (state_ == kConnected) {
        setStata(kdisConnected);                // 设置连接的状态
        channel_->disableAll();                 // 将Channel的关注事件清零
        connectioncb_(shared_from_this());      // 执行回调函数
    }
    channel_->remove();
}

void TcpConnection::handleRead(TimeStamp receiveTime) {
    //进行数据的读取，根据返回值执行不同的回调函数， 使用buffer进行读取
    int saveErrno = 0;                          // 用于取出来Buffer中的errno
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0) {                                // 读到了用户的数据，执行cb
        // 读事件发生，调用用户的回调
        messagecb_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();                          // 客户端断开了
    } else {
        //TODO : log 
        errno = saveErrno;
        handleError();                          // 数据出错
    }
}

void TcpConnection::handleWrite() {
    //暂时不写
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    channel_->disableAll();
    closecb_(shared_from_this());
}

void TcpConnection::handleError() {
    int optval;
    socklen_t optlen = sizeof optval;
    int err = 0;
    if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        err = errno;
    } else {
        err = optval;
    }

    printf("TcpConnection::handleError name:%s - SO_ERROR:%d \n", name_.c_str(), err);
}