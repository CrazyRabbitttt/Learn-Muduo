#include "Learn-Muduo/Net/Accepter.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/InetAddress.h"

using namespace bing;

void NewConnection(int sockfd, const InetAddress& peeraddr) {
    printf("accpet a new connection from %s\n", peeraddr.toIpPort().c_str());
    ::write(sockfd, "How are you?\n", 13);
    ::close(sockfd);
}

void NewConnection2(int sockfd, const InetAddress& peeraddr) {
    printf("accept a new connection ffff %s\n", peeraddr.toIpPort().c_str());
    ::write(sockfd, "How are mee?\n", 13);
    ::close(sockfd);
}


int main() {
    printf("main pid = %d\n", getpid());
    InetAddress localaddr(9982);
    InetAddress localaddr2(9981);
    EventLoop loop;

    Accepter acctor(&loop, localaddr);
    Accepter acctor2(&loop, localaddr2);
    
    acctor2.setNewConnectioncallback(NewConnection2);
    acctor.setNewConnectioncallback(NewConnection);     //listen socket读事件触发后进行accept + 执行这个用户指定的回调函数

    acctor2.listen();
    acctor.listen();                                    //listen, 往epoll注册读事件

    loop.loop();                                        //loop

}