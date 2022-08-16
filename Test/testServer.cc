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
    conn->send("建立连接喽！\n");
}


void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp receiveTime){
    printf("onMessage() : reveived %zd bytes from connection [%s]]]] at %s\n",
        buffer->readableBytes(), conn->name().c_str(), receiveTime.toString().c_str());

    const std::string tmp  = (buffer->retrieveAllAsString());
    std::string str = "肖冰说:";
    str += tmp;
    conn->send(str);
}



int main() {
    printf("Main(): pid = %d\n", getpid());

    const InetAddress listenAddr(9981);
    EventLoop loop;                     
    std::string name = "TestServer";    
    TcpServer server(&loop, listenAddr, name);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}