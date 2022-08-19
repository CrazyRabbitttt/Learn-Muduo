#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/TcpServer.h"
#include <functional>
#include <string>
#include <iostream>

using namespace bing;
/*
    进行Server的设置
    创建Eventloop
    明确Server的参数
    构造函数处理onConnection & onMessage 
*/


class ChatServer {
 public:
    ChatServer(EventLoop* loop,
                const InetAddress& listenAddr,
                const std::string &nameArg)
            : server_(loop, listenAddr, nameArg), loop_(loop)
        {

            server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));

            server_.setMessageCallback(std::bind(&ChatServer::onMessage, this, std::placeholders::_1,
                                                                             std::placeholders::_2, std::placeholders::_3));

            // 1个io线程，3个工作的线程
            server_.setThreadNum(4);
        }

        void start() {
            server_.start();
        }

 private:

    void onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        std::cout << conn->peerAddress().toIpPort() << "-> "
            << conn->localAddress().toIpPort() << " state:online " << std::endl;      
    } else {        // 连接断开的处理
        printf("onConnection(): connection [%s] is done\n",
                conn->name().c_str());
        conn->shutdown();
    }
    }


    void onMessage(const TcpConnectionPtr& conn,
                                Buffer* buffer,
                                TimeStamp time)
    {
        std::string buf = buffer->retrieveAllAsString();
        std::cout << "recv data:" << buf << "time:" << time.toString() << std::endl;
        buf += std::to_string(currentThread::tid());
        conn->send(buf);
    }

    TcpServer server_;
    EventLoop* loop_;
};

int main() {
    EventLoop loop;
    InetAddress addr(2002);

    ChatServer server(&loop, addr, "ChatServer");
    server.start();
    loop.loop();

    exit(0);
}
