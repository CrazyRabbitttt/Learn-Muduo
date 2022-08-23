#include "Learn-Muduo/Http/httpServer.h"
#include "Learn-Muduo/Http/httpContent.h"
#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/TcpConnection.h"
#include <functional>

using namespace bing;

HttpServer::HttpServer(EventLoop* loop, const InetAddress& address)
    :loop_(loop), server_(loop, address, "HttpServer")
{
    server_.setConnectionCallback(std::bind(&HttpServer::ConnectionCallback, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&HttpServer::MessageCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server_.setThreadNum(kThreaNums);
    setHttpResponseCallback(std::bind(&HttpServer::HttpDefaultCallback, this, std::placeholders::_1, std::placeholders::_2));
}    
HttpServer::~HttpServer() {}

void HttpServer::MessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp time) {
    printf("Message call back!\n");
    HttpContent* content = conn->getHttpContent();
    if (!content->ParseContent(buffer)) {
        printf("Parse Http content error!\n");
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    printf("Parse Http Request sucess!\n");
    content->show();
    // 解析HTTP请求成功
    if (content->GetCompleteRequest()) {
        printf("Now will deal with Request\n");
        dealWithRequest(content->request(), conn);
        content->ResetContentState();
    }
}

void HttpServer::dealWithRequest(const HttpRequest& request, const TcpConnectionPtr& conn) {
    string connection_state = std::move(request.GetHeader("Connection"));
    bool close = (connection_state == "Close" ||
                (request.version() == kHttp10 && 
                 connection_state != "Keep-Alive"));
    HttpResponse response(close);
    response_callback_(request, response);
    Buffer buffer;
    response.AppendToBuffer(&buffer);
    conn->send(&buffer);
}

