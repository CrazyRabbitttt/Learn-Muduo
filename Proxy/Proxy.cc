#include "tuunel.h"

#include <iostream>
#include <string.h>
#include <string>
#include <map>

InetAddress* g_serverAddr;                   
EventLoop* g_eventLoop;
std::map<std::string, TunnelPtr> g_tunnels;

using namespace muduo;

// Proxy收到连接请求
void onServerConnection(const TcpConnectionPtr& conn) {
    printf("New Connection from %s\n", conn->peerAddress().toIpPort().c_str());
    if (conn->connected()) {
        conn->setTcpNoDelay(true);
        conn->stopRead();               // 连接建立了，不用监听socket的读事件了
        TunnelPtr tunnel(new Tunnel(g_eventLoop, *g_serverAddr, conn));
        tunnel->setup();
        tunnel->connection();           // proxy同服务器进行连接
        g_tunnels[conn->name()] = tunnel;
    } else {
        assert(g_tunnels.find(conn->name()) != g_tunnels.end());
        g_tunnels[conn->name()]->disconnection();
        g_tunnels.erase(conn->name());
    }
}

// Proxy 接收到客户端的信息
void onServerMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp) {
    printf("Receive the message from server..\n");
    if (!conn->getContext().empty()) {              // 也就是有连接？
        const TcpConnectionPtr& clientConn = boost::any_cast<const TcpConnectionPtr&>(conn->getContext());;
        clientConn->send(buffer);                   // 将获得的client的数据传送到server 
    }   
}



int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: %s <ip> <port> <listenport>\n", argv[0]);
        return 0;
    }

    const char* Ip = argv[1];                                      // Server Ip
    uint16_t port  = static_cast<uint16_t>(atoi(argv[2]));        // Server Port 
    InetAddress serverAddr(port, Ip);
    g_serverAddr = &serverAddr;

    uint16_t proxyPort = static_cast<uint16_t>(atoi(argv[3]));
    InetAddress proxyAddr(proxyPort);

    EventLoop loop;                 // Proxy的EventLoop
    g_eventLoop = &loop;

    TcpServer server(&loop, proxyAddr, "Proxy");

    server.setConnectionCallback(onServerConnection);
    server.setMessageCallback(onServerMessage);

    server.start();

    loop.loop();
}
