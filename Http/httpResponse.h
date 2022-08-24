#ifndef BING_HTTPRESPONSE_H
#define BING_HTTPRESPONSE_H

#include <string>

using std::string;

namespace bing {

static const string CRLF = "\r\n";

enum HttpStatusCode {
    k100Continue   = 100,
    k200OK         = 200,
    k400BadRequest = 400,
    k403Forbidden  = 403,
    k404NotFound   = 404,
    k500InternalServerErrno = 500
};

class Buffer;

// 储存返回的Response，提供设置参数的函数
class HttpResponse {

 public:
    HttpResponse(bool close_connection)
        : type_("text/html"),              // 按照纯文本的方式传送回去
          close_connection_(close_connection) {}
    ~HttpResponse() {}

    void setStatusCode(HttpStatusCode status_code) { status_code_ = status_code; }
    void setStatusMessage(const string& status_message) { status_message_ = std::move(status_message); }
    void setCloseConnection(bool close_connection) { close_connection_ = close_connection; }

    void setBodyType(const string& type) { type_ = std::move(type); }
    void setBodyType(const char* type) { type_ = type; }
    void setBody(const string& body) { body_ = std::move(body); }
    void setBody(const char* body) { body_ = body; }
    void AppendToBuffer(Buffer* buffer);        // 将response整合 & 回送？

    bool CloseConnection() const { return close_connection_; }

 private:
    static const string  server_name_;
    static const string server_http_version_;
    HttpStatusCode status_code_;
    string status_message_;
    string headers_;
    string body_;
    string type_;                               // 返回给客户端，本次的数据是一种什么样的格式
    bool close_connection_;                     // 连接是否是建立的  

};

}   // namespace bing

#endif