#include "Learn-Muduo/Net/Socket.h"
#include "Learn-Muduo/Net/InetAddress.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>


using namespace bing;


void Socket::setNoblock() {
    int fd = sockfd_;
    int old_options = fcntl(sockfd_, F_GETFL);
    int new_options = old_options | O_NONBLOCK;
    fcntl(sockfd_, F_SETFL, new_options);  
}


Socket::~Socket() {
    close(sockfd_);
}

void Socket::listen() {
    printf("开始进行监听\n");
    if (::listen(sockfd_, 1024) < 0) {
        printf("listen sockfd faild: %d\n", sockfd_);
    }
}

void Socket::bindAddr(const InetAddress& localaddr) {
    if (::bind(sockfd_, (sockaddr*)localaddr.getSockAddr(), sizeof (sockaddr_in)) < 0) {
        printf("bind sockfd error : %d\n", sockfd_);
    }
}

void Socket::shutDownWrite() {
    //将写端进行关闭
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        printf("shutdown write error: %d\n", sockfd_);
    }
}


int Socket::accept(InetAddress* peeraddr) {
    //进行socket原语的封装  
    printf("Socket::accept被调用\n");
    sockaddr_in addr;
    socklen_t len = sizeof addr;
    bzero(&addr, len);

    // setNoblock();       //将sockfd 设置为 non-block 
    // int connfd = ::accept(sockfd_, (sockaddr*)&addr, &len);
    int connfd = ::accept4(sockfd_, (sockaddr*)&addr, &len, SOCK_NONBLOCK);
    if (connfd >= 0) {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

//能够绑定处于time_wait状态的地址
void Socket::setReuseAddr(bool on) {
    int statval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &statval, sizeof statval);
} 


void Socket::setReusePort(bool on) {
    int statval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &statval, sizeof statval);
}

void Socket::keepAlive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
}

