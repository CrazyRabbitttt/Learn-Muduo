#ifndef BING_HTTPSERVER_H
#define BING_HTTPSERVER_H

#include "Learn-Muduo/Base/CountDownLatch.h"
#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Http/httpRequest.h"
#include "Learn-Muduo/Http/httpResponse.h"
#include "Learn-Muduo/Log/logger.h"


namespace bing {

class EventLoop;
static const int kThreaNums = 4;
static const int kIdleConnectionTimeOut = 10;        // 超时踢掉的时间


class HttpServer {
    using HttpResponseCallback = std::function<void (const HttpRequest&, HttpResponse&)>;
 public:
    HttpServer(EventLoop* loop, const InetAddress& addr, bool closeIdleConn);
    ~HttpServer();

    void start() {
        server_.start();
    }
                            // weak ptr to judge if the connection alive
    void HandleIdleConnection(std::weak_ptr<TcpConnection>& connection);


    void HttpDefaultCallback(const HttpRequest& request, HttpResponse& response)  {
        response.setStatusCode(k404NotFound);
        response.setStatusMessage("Not Found");
        response.setCloseConnection(true);
    }

    void setHttpResponseCallback(const HttpResponseCallback& cb) {
        response_callback_ = std::move(cb);
    }

    void dealWithRequest(const HttpRequest& request, const TcpConnectionPtr& conn);

    void ConnectionCallback(const TcpConnectionPtr& conn);
    
    // void MessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp time);
    void MessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp time);

 private:
    EventLoop* loop_;
    TcpServer server_;
    // CountDownLatch latch_;

    bool auto_close_idleconnection_;

    HttpResponseCallback response_callback_;

};

}   // namespace bing 

#endif