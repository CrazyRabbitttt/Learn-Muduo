#include "Learn-Muduo/Net/Accepter.h"
#include "Learn-Muduo/Net/InetAddress.h"
#include "Learn-Muduo/Net/Channel.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace bing;

//创建listen socket
static int createNonBlockFd() {
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        printf("create sockfd error\n");
    }
    return fd;
}


Accepter::Accepter(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
    :loop_(loop), 
    acceptSocket_(createNonBlockFd()),               //make socket
    acceptChannel_(loop, acceptSocket_.fd()),       //对应的fd是listenfd
    listening_(false)
    {
        /*
            Accept进行连接的监听，如果有新用户进行了连接，进行用户
            的回调。将connfd封装成Channel，唤醒subLoop进行处理
        */

        acceptSocket_.setReuseAddr(true);
        acceptSocket_.setReusePort(true);
        acceptSocket_.bindAddr(listenAddr);     //socket bind the addr

        //监听socket的回调函数就是handleRead, 加入this就是绑定对象的成员函数
        acceptChannel_.setReadCallBack(std::bind(&Accepter::handleRead, this));
    }

Accepter::~Accepter() {
    acceptChannel_.disableAll();    //什么都不关注了，
    acceptChannel_.remove();        //从Event中移除
}


//进行监听
void Accepter::listen() {
    listening_ = true;
    acceptSocket_.listen();                 //listen
    printf("will register read\n");
    printf("监听的描述符号:%d\n", acceptChannel_.fd());
    acceptChannel_.enableReading();     //监听读事件, 跟loop进行了链接了
}

//监听到了socket的连接了，进行accept 
void Accepter::handleRead() {

    printf("监听到了连接，下面进行处理\n");
    InetAddress clientAddr(0);      //默认的使用0

    int connfd = acceptSocket_.accept(&clientAddr); //accept后会填充对端的地址
    
    if (connfd >= 0) {      //连接创建成功了
        //用户的回调函数
        printf("连接创建成功，进行回调函数\n");
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

