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
    setState(kConnected);
    channel_->enableReading();      //注册可读， socket
    //调用callback 
    connectioncb_(shared_from_this());
}

//进行连接的销毁
void TcpConnection::connectDestoryed() {
    if (state_ == kConnected) {
        setState(kdisConnected);                // 设置连接的状态
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


// 发送数据
void TcpConnection::send(const std::string& message) {
    // 如果已经是连接了的
    if (state_ == kConnected) {
        // 是否是在当前的线程
        if (loop_->isInLoopThread()) {
            printf("直接本线程发送\n");
            printf("传给SendInloop的参数:%s\n", message.c_str());
            sendInloop(message.c_str(), message.size());
        } else {        //不是的话，转到loop_所在的线程进行发送
            printf("转到IO线程去执行\n");
            loop_->runInLoop(std::bind(
                &TcpConnection::sendInloop,
                 this,
                 message.c_str(),
                 message.size()));
        }
    }
}


/*
    发送数据，把等待发送的数据写入到缓冲区中去
*/
void TcpConnection::sendInloop(const void* data, size_t len) {
    ssize_t nwrite = 0;
    ssize_t remain = len;     //剩余的还没有发送的数据
    bool faultError = false;


    // 断开连接的状态， 不能再发送了，之前调用了shutdown()
    if (state_ == kdisConnected) {
        printf("TcpConnection::sendinloop, can not write, connection is done!\n");
        return;
    }
    printf("SendInloop传过来的参数: %s\n", (char*)data);
    // channel_第一次写数据(epoll没关注写时间)，缓冲区不能有待发送的数据
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        printf("开始运行发送数据了, [%s]\n", (char*)data);
        nwrite = ::write(channel_->fd(), data, len);
        if (nwrite >= 0) {       
            // 发送成功了
            printf("发送部分成功! fd = %d, bytes : %d\n", channel_->fd(), nwrite);
            remain = len - nwrite;
            if (remain == 0 && writeCompeletecb_) {
                // 这里直接发送成功了，不需要设置epollout | handwrite了
                loop_->queueInLoop(std::bind(writeCompeletecb_, shared_from_this()));
            }
        } else {
            // 出错
            nwrite = 0;
            // EWOULDBLOCK:非阻塞IO，还没有数据导致的返回
            if (errno != EWOULDBLOCK) {
                //TODO:log
                printf("error, TcpConnection::sendInloop");
                // 如果是因为连接重置？
                if (errno == EPIPE || errno == ECONNREFUSED) {
                    faultError = true;
                }
            }

        }
    }

    /*
        这一次数据没有全部发送出去，剩余的数据需要写到缓冲区中去, 注册channel-的epollout事件
        缓冲区有数据Epoll会唤醒对应的writecallback, 也就是调用handlewrite 
    */
    if (!faultError && remain > 0) {
        size_t oldlen = outputBuffer_.readableBytes();

        //将剩余的数据写入buffer中
        outputBuffer_.append((char*)data + nwrite, remain);
        if (!channel_->isWriting()) {   // 如果没有关注写的事件
            channel_->enableWriting();  // 注册写事件
        }
    }

}


void TcpConnection::handleWrite() {
    // 如果关注了写事件
    if (channel_->isWriting()) {
        int saveErrno = 0;
        ssize_t n = outputBuffer_.writeFd(channel_->fd(), &saveErrno);
        if (n > 0) {
            // 如果写成功了，就更新一下buffer中指针的位置
            outputBuffer_.retrieve(n);
            // 发送完成
            if (outputBuffer_.readableBytes() == 0) {
                // 不再关注写事件
                channel_->disableWriting();
                // 执行写成功的回调函数
                if (writeCompeletecb_) {
                    // 唤醒loop对应的线程执行回调
                    loop_->queueInLoop(std::bind(writeCompeletecb_, shared_from_this()));
                }
                if (state_ == kDisConnecting) {
                    shutdownInloop();
                }
            }
        } else {
            // 出错
            printf("TcpConnection::handleWrite");
        }
    } else {
        // 没有关注写事件
        printf("TcpConnection fd=%d is down, no more writing \n", channel_->fd());
    }

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


// 关闭连接
void TcpConnection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisConnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInloop, this));
    }
}

void TcpConnection::shutdownInloop() {
    // 如果没有关注写事件，说明已经是将发送缓冲区发送完了
    if (!channel_->isWriting()) {
        // 关闭写端
        socket_->shutDownWrite();
    }
}
