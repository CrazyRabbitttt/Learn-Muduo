#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/InetAddress.h"
#include "Learn-Muduo/Net/TcpConnection.h"
#include <stdio.h>
#include <signal.h>

std::string message;

using namespace bing;
void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        printf("onConnection(): new Connection [%s] from %s\n", 
                conn->name().c_str(), 
                conn->peerAddress().toIpPort().c_str());
        conn->send(message);
    } else {        // 连接断开的处理
        printf("onConnection(): connection [%s] is done\n",
                conn->name().c_str());
    }
}

void onWriteComplete(const TcpConnectionPtr& conn) {
    // 如果完全写成功了？？？？那么就会不断的写啊
    conn->send(message);
}


void onMessage(const TcpConnectionPtr& conn,
                Buffer* buffer, 
                TimeStamp receiveTime)
{
    printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buffer->readableBytes(),
         conn->name().c_str(),
         receiveTime.toString().c_str());
    buffer->retrieveAll();      // 将指针的位置清零， 不管接受的数据？？？
}

int main() {
    printf("Main(): pid = %d\n", getpid());

    ::signal(SIGPIPE, SIG_IGN);

    std::string line;
    for (int i = 33; i < 127; ++i) {
        line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127-33; ++i) {
        message += line.substr(i, 72) + '\n';
    }


    InetAddress localAddr(9981);
    EventLoop loop;

    TcpServer server(&loop, localAddr, "Server10");
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setWriteComCallback(onWriteComplete);
    server.start();

    loop.loop();

    return 0;
}

