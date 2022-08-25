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
      peeraddr_(peerAddr), 
      highWaterMark_(64 * 1024 * 1024)
    {
        //进行可读事件的回调， socket上的可读事件被触发了
        channel_->setReadCallBack(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));      //绑定，传送参数
        channel_->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
        channel_->setErrorCallBack(std::bind(&TcpConnection::handleError, this));
        channel_->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));
    }

TcpConnection::~TcpConnection() {
    // printf("~TcpConnection, fd = %d\n", channel_->fd());
}

void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->enableReading();      //注册可读， socket
    //调用Connectioncallback 
    connectioncb_(shared_from_this());
}

//进行连接的销毁
void TcpConnection::connectDestoryed() {
    if (state_ == kConnected) {
        setState(kdisConnected);                // 设置连接的状态
        channel_->disableAll();                 // 将Channel的关注事件清零
        connectioncb_(shared_from_this());      // 执行回调函数
    }
    // 将channel从EPOLLER中删除掉
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
        handleClose();                          // read = 0, 客户端断开了, 唯一的断开连接的方式
    } else {
        //TODO : log 
        errno = saveErrno;
        handleError();                          // 数据出错
    }
}


void TcpConnection::send(const string& message) {
    int remain = message.size();
    int send_size = 0;
    // channel_第一次写数据(epoll没关注写时间)，缓冲区不能有待发送的数据
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        send_size = static_cast<int>(::write(channel_->fd(), message.data(), message.size()));
        if (send_size >= 0) {
            remain -= send_size;
        } else {
            if (errno != EWOULDBLOCK) {
                printf("TcpConnection::Send write failed\n");
            }
            return ;
        }
    }

    assert(remain <= message.size());
    if (remain > 0) {
        // 将剩余的数据写入buffer中, 要注意添加到之前还剩余的数据的后面
        outputBuffer_.append((char*)message.c_str() + send_size, remain);
        if (!channel_->isWriting()) {   // 如果没有关注写的事件
            printf("数据没发送完，放到buffer中，注册写事件\n");
            channel_->enableWriting();  // 注册写事件
        }
    }
}

void TcpConnection::send(Buffer* buffer) {    
    if (state_ == kConnected) {
        send(std::move(string(buffer->peek(), buffer->readableBytes())));
        buffer->retrieveAll();
    }
}



// 发送数据没发送完，写到缓冲区中了，回调函数(HandleWrite)就是将缓冲区中的数据发送
void TcpConnection::handleWrite() {
    // 如果关注了写事件
    if (channel_->isWriting()) {        // 前提肯定是需要关注了写的事件啦
        int saveErrno = 0;
        printf("触发了写事件，将缓冲区的内容写到对端\n");
        ssize_t n = outputBuffer_.writeFd(channel_->fd(), &saveErrno);
        if (n > 0) {    
            // 写成功了， 更新一下缓冲区的指针
            outputBuffer_.retrieve(n);
            // 如果全部都发送完成了， 从epoll中注销掉写事件, 执行写成功的回调函数
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                // 用不到写成功的回调函数
                // if (writecompeletecb_) {
                //     // 唤醒loop对应的线程，执行写成功的回调函数
                //     printf("Writecompletecallback\n");
                //     loop_->queueInLoop(std::bind(writecompeletecb_, shared_from_this()));
                // }
                if (state_ == kDisConnecting) {
                    shutdownInloop();
                }
            }
        } else {
            // 写出错， 
            printf("TcpConnection::hanldWrite error\n");
        }
    } else {
        // 没有关注写事件
        printf("TcpConnection fd=%d is down, no more writing \n", channel_->fd());
    }
}


void TcpConnection::handleClose() {
    setState(kdisConnected);        // 设置状态为DisConnected
    channel_->disableAll();

    TcpConnectionPtr connPtr(shared_from_this());
    connectioncb_(connPtr);         // 执行回调函数， 关闭连接的执行
    closecb_(connPtr);              // 执行TcpServer::removeConnection, 最终DestoryConnection 
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


// 关闭连接
void TcpConnection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisConnecting);       // 设置一下状态
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInloop, this));
    }
}

void TcpConnection::shutdownInloop() {
    // 如果没有关注写事件，说明已经是将发送缓冲区发送完了， 也就是shutdown保证能够将数据发送出去再关闭连接
    if (!channel_->isWriting()) {
        // 关闭写端
        socket_->shutDownWrite();
    }
}

void TcpConnection::shutdownRead() {
    if (state_ == kConnected) {
        setState(kDisConnecting);
        socket_->shutDownRead();
    }
}

