#ifndef BING_HTTPSERVER_H
#define BING_HTTPSERVER_H


#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Http/httpRequest.h"
#include "Learn-Muduo/Http/httpResponse.h"



namespace bing {

class EventLoop;
static const int kThreaNums = 8;

class HttpServer {
    using HttpResponseCallback = std::function<void (const HttpRequest&, HttpResponse&)>;
 public:
    HttpServer(EventLoop* loop, const InetAddress& addr);
    ~HttpServer();

    void start() {
        server_.start();
    }

    void HttpDefaultCallback(const HttpRequest& request, HttpResponse& response)  {
        response.setStatusCode(k404NotFound);
        response.setStatusMessage("Not Found");
        response.setCloseConnection(true);
    }

    void setHttpResponseCallback(const HttpResponseCallback& cb) {
        response_callback_ = std::move(cb);
    }


    void dealWithRequest(const HttpRequest& request, const TcpConnectionPtr& conn);


    void ConnectionCallback(const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            printf("%s -> %s state:online \n", conn->peerAddress().toIpPort().c_str(), conn->localAddress().toIpPort().c_str());
        } else {
            printf("%s -> %s state:offline \n", conn->peerAddress().toIpPort().c_str(), conn->localAddress().toIpPort().c_str());
            conn->shutdown();       // 也就是关闭连接
        }
    }
    
    // void MessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp time);
    void MessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp time);

 private:
    EventLoop* loop_;
    TcpServer server_;

    HttpResponseCallback response_callback_;

};

}   // namespace bing 

#endif