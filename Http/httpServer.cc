#include "Learn-Muduo/Http/httpServer.h"
#include "Learn-Muduo/Http/httpContent.h"
#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/TcpConnection.h"
#include "Learn-Muduo/Log/logstream.h"
#include <functional>

using namespace bing;

HttpServer::HttpServer(EventLoop* loop, const InetAddress& address, bool closeIdleConn)
    :loop_(loop), server_(loop, address, "HttpServer"), auto_close_idleconnection_(closeIdleConn)
{
    server_.setConnectionCallback(std::bind(&HttpServer::ConnectionCallback, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&HttpServer::MessageCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server_.setThreadNum(kThreaNums);
    setHttpResponseCallback(std::bind(&HttpServer::HttpDefaultCallback, this, std::placeholders::_1, std::placeholders::_2));
    LOG_INFO << "HttpServer Running...";
}    
HttpServer::~HttpServer() {}

void HttpServer::MessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp time) {
    if (auto_close_idleconnection_)  {
        conn->updateTimeStamp(TimeStamp::now());        // 来消息了，更新一下stamp的最后接收消息的时间戳
    }
    if (conn->IsShutdown()) return ;
    HttpContent* content = conn->getHttpContent();
    if (!content->ParseContent(buffer)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    // 解析HTTP请求成功
    if (content->GetCompleteRequest()) {
        dealWithRequest(content->request(), conn);  
        content->ResetContentState();
    }
}

void HttpServer::HandleIdleConnection(std::weak_ptr<TcpConnection>& connection) {
    // 用weak_ptr 去探测一下shared_ptr的对象是不是还活着
    TcpConnectionPtr conn(connection.lock());
    if (conn) {
            // 超时了，因为最后的时间 ➕ 超时间隔小于now, 就是超时了，关掉连接
        if (TimeStamp::AddTime(conn->timestamp(), kIdleConnectionTimeOut) < TimeStamp::now()) {
            conn->shutdown();
        } else {    // 否则重置超时时间
            loop_->RunAfter(kIdleConnectionTimeOut, std::bind(&HttpServer::HandleIdleConnection, this, connection));
        }
    }
}

void HttpServer::ConnectionCallback(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        // LOG_INFO << TimeStamp::now().toString() << " " << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:online "; 
        printf("%s  %s -> %s state:online \n", TimeStamp::now().toString().c_str(), conn->peerAddress().toIpPort().c_str(), conn->localAddress().toIpPort().c_str());
        if (auto_close_idleconnection_) {
            loop_->RunAfter(kIdleConnectionTimeOut, std::bind(&HttpServer::HandleIdleConnection, this, std::weak_ptr<TcpConnection>(conn)));
        }    
    } else {
        // LOG_INFO << TimeStamp::now().toString() << " " << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:offline "; 
        printf("%s  %s -> %s state:offline \n", TimeStamp::now().toString().c_str(), conn->peerAddress().toIpPort().c_str(), conn->localAddress().toIpPort().c_str());
            // conn->shutdown();       // 也就是关闭连接
    }
}


void HttpServer::dealWithRequest(const HttpRequest& request, const TcpConnectionPtr& conn) {
    string connection_state = std::move(request.GetHeader("Connection"));
    bool close = (connection_state == "Close" ||
                (request.version() == kHttp10 && 
                 connection_state != "Keep-Alive"));
    HttpResponse response(close);                           // make the response, 
    response_callback_(request, response);
    Buffer buffer;
    response.AppendToBuffer(&buffer);
    conn->send(&buffer);
    if (response.CloseConnection()) {    // 不是长连接， 关闭连接但是可能数据发送有问题
        conn->shutdown();
        // conn->shutdownRead();           // 关闭读端，写端不关闭
    }
}


