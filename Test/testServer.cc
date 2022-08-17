#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/InetAddress.h"
#include "Learn-Muduo/Net/TcpConnection.h"
#include <stdio.h>
using namespace bing;

void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        printf("onConnection() : tid = %d new connection [%s] from %s\n",
        currentThread::tid(),
        conn->name().c_str(),
        conn->peerAddress().toIpPort().c_str());
    } else {        // 这里就是最终的用户进行处理断开连接的地方
        printf("onConnection : tid = %d connection [%s] is done\n", currentThread::tid(), conn->name().c_str());
    }
    std::string tmp = "建立连接喽! 处理连接的线程id:\n";
    tmp += std::to_string(currentThread::tid());
    conn->send(tmp);   
}


void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp receiveTime) {
    printf("onMessage() : tid = %d reveived %zd bytes from connection [%s] at %s\n",
        currentThread::tid(),
        buffer->readableBytes(), conn->name().c_str(), receiveTime.toString().c_str());

    const std::string tmp  = (buffer->retrieveAllAsString());
    printf("%s", tmp.c_str());
    std::string str = "肖冰说:";
    str += tmp;
    conn->send(str);
}



int main(int argc, char* argv[]) {
    printf("Main(): pid = %d\n", getpid());

    const InetAddress listenAddr(9981);
    EventLoop loop;                     
    std::string name = "TestServer";    
    TcpServer server(&loop, listenAddr, name);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    if (argc > 1) {
        server.setThreadNum(atoi(argv[1]));
    }
    server.start();

    loop.loop();
}
