#include "httpResponse.h"
#include "Buffer.h"

using namespace bing;

const string HttpResponse::server_name_ = "Bing_muduo";
const string HttpResponse::server_http_version_ = "HTTP/1.1";

/*
    Connection: close\r\n

    Content-Length: \r\n
    Connection: Keep-Alive\r\n
    Content_Type:type_\r\n
    Server: server_name\r\n


*/

void HttpResponse::AppendToBuffer(Buffer* buffer) {
    string message;
    // 状态： 版本 状态码 状态信息
    message += (server_http_version_ + " " + std::to_string(status_code_) + " " + status_message_ + CRLF);
    if (close_connection_) {
        message += ("Connection: close" + CRLF);
    } else {
        message += ("Content_Length: " + std::to_string(body_.size()) + CRLF);
        message += ("Connection: Keep-Alive" + CRLF);
        message += ("Content_Type: " + type_ + CRLF);
        message += ("Server: " + server_name_ + CRLF);
    }

    message += CRLF;
    message += body_;

    buffer->append(message);
    return;
}