#include "Learn-Muduo/Net/Accepter.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/InetAddress.h"

using namespace bing;

void NewConnection(int sockfd, const InetAddress& peeraddr) {
    printf("accpet a new connection from %s\n", peeraddr.toIpPort().c_str());
    ::write(sockfd, "How are you?\n", 13);
    ::close(sockfd);
}


int main() {
    printf("main pid = %d\n", getpid());
    InetAddress localaddr(9981);

    EventLoop loop;

    Accepter acctor(&loop, localaddr);

    acctor.setNewConnectioncallback(NewConnection);

    acctor.listen(); 

    loop.loop();
}