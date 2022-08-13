#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/InetAddress.h"
#include "Learn-Muduo/Net/TcpConnection.h"
#include <stdio.h>
using namespace bing;

void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        printf("onConnection() : new connection [%s] from %s\n",
        conn->name().c_str(),
        conn->peerAddress().toIpPort().c_str());
    } else {
        printf("onConnection : connection [%s] is done\n",
        conn->name().c_str());
    }
}


void onMessage(const TcpConnectionPtr& conn, const char* data, ssize_t len){
    printf("onMessage() : reveived %zd bytes from connection [%s]\n",
        len, conn->name().c_str());
}



int main() {
    printf("Main(): pid = %d\n", getpid());

    const InetAddress listenAddr(9981);
    EventLoop loop;                         
    TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}