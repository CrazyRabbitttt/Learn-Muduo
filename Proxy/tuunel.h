#ifndef BING_PROXY_TUNNEL_H
#define BING_PROXY_TUNNEL_H


#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpClient.h"
#include "muduo/net/TcpServer.h"
#include "muduo/base/Timestamp.h"

using namespace muduo::net;
using namespace muduo::detail;


class Tunnel : public std::enable_shared_from_this<Tunnel> {
 public:
    // 保存好了同server通信的clent_ & 同client通信的serverConn
   Tunnel(EventLoop* loop, const InetAddress& serverAddr, const TcpConnectionPtr& serverConn)
        : client_(loop, serverAddr, serverConn->name()),
          serverConn_(serverConn)
        {
          // LOG_INFO << "Tunnel " << serverConn_->peerAddress().toIpPort() << " <-> " 
                  //  << serverAddr.toIpPort(); 
        }
      
   void setup() {
      using std::placeholders::_1;
      using std::placeholders::_2;
      using std::placeholders::_3;

      client_.setConnectionCallback(std::bind(&Tunnel::onClientConnection, shared_from_this(), _1));

      client_.setMessageCallback(std::bind(&Tunnel::onClientMessage, shared_from_this(), _1, _2, _3));

      // highwatercallback...


   }

   void connection() {
      client_.connect();          // 直接进行同服务器的连接
   }

   void disconnection() {
      client_.disconnect();
   }

   ~Tunnel() { /*LOG_INFO << "~Tunnel";*/ }

 private:

    void teardown()
  {
    client_.setConnectionCallback(muduo::net::defaultConnectionCallback);
    client_.setMessageCallback(muduo::net::defaultMessageCallback);
    if (serverConn_)
    {
      serverConn_->setContext(boost::any());
      serverConn_->shutdown();
    }
    clientConn_.reset();
  }

    // 将自身看作是client，同Server进行连接的建立
    void onClientConnection(const TcpConnectionPtr& conn) {
        using std::placeholders::_1;
        using std::placeholders::_2;
        LOG_DEBUG << (conn->connected() ? "UP" : "DOWN");
        if (conn->connected()) {
          conn->setTcpNoDelay(true);      // nodelay 算法
          serverConn_->setContext(conn);
          serverConn_->startRead();
          clientConn_ = conn;

          // 刚建立连接就带数据
          if (serverConn_->inputBuffer()->readableBytes() > 0)
          {
            conn->send(serverConn_->inputBuffer());
          }
        } else {                        // 如果说是BadRequest Or 短连接，关闭代理对应的连接
            teardown();                 // 关闭掉连接
        }
    }

    void onClientMessage(const TcpConnectionPtr& conn, Buffer* buffer, muduo::Timestamp) {
      LOG_DEBUG << conn->name() << " " << buffer->readableBytes();
      if (serverConn_) {
        serverConn_->send(buffer);      // 收到了数据就发送给客户端
      } else {
        buffer->retrieveAll();          // 没有连接了就读出来
        abort();
      }
    }

    TcpClient client_;                  // 客户端
    TcpConnectionPtr serverConn_;       // 作为Server的一个连接
    TcpConnectionPtr clientConn_;       // 作为Client的一个连接
};

using TunnelPtr = std::shared_ptr<Tunnel>;

#endif 