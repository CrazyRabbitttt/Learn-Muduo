#include "Accepter.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace bing;

//创建listen socket
static int createNonBlockFd() {
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        printf("create sockfd error\n");
    }
    return fd;
}


Accepter::Accepter(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
    :loop_(loop), 
    acceptSocket_(createNonBlockFd()),                  //创建socket传递给Socket_
    // acceptChannel_(loop, acceptSocket_.fd()),       //用Socket_.fd构造Channel_, 没传过去fd????
    listening_(false)
    {
        acceptChannel_.Init(loop, acceptSocket_.fd());      //放弃构造，使用init函数传递fd
        /*
            Accept进行连接的监听，如果有新用户进行了连接，进行用户
            的回调。将connfd封装成Channel，唤醒subLoop进行处理, (runinloop?执行establish)
        */

        acceptSocket_.setReuseAddr(true);
        // acceptSocket_.setReusePort(true);
        acceptSocket_.bindAddr(listenAddr);     //socket bind the addr

        //监听socket的回调函数就是handleRead, 加入this就是绑定对象的成员函数，如果说可读就触发
        acceptChannel_.setReadCallBack(std::bind(&Accepter::NewConnection, this));
    }

Accepter::~Accepter() {
    acceptChannel_.disableAll();    //什么都不关注了，
    acceptChannel_.remove();        //从Event中移除
}


//进行监听
void Accepter::listen() {
    listening_ = true;
    acceptSocket_.listen();                 //listen
    acceptChannel_.enableReading();     //监听读事件, 跟loop进行了链接了
}

//监听到了socket的连接了，进行accept 
void Accepter::NewConnection() {

    loop_->assertInLoopThread();
    InetAddress clientAddr;      //默认的使用0

    int connfd = acceptSocket_.accept(&clientAddr); //accept后会填充对端的地址
    
    if (connfd >= 0) {      //连接创建成功了
        //用户的回调函数
        if (newConnectioncb_) {
            newConnectioncb_(connfd, clientAddr);
        } else {
            ::close(connfd);
        }
    } else {
        printf("handleRead error, accpet error\n");
        if (errno == EMFILE) {
            printf("文件描述符达到了上限\n");
        }
    }
    
}

