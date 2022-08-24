#ifndef BING_HTTPCONTENT_H
#define BING_HTTPCONTENT_H

#include "Learn-Muduo/Http/httpRequest.h"
#include "Learn-Muduo/Http/httpResponse.h"
#include <string.h>

namespace bing {

// 主状态机的操控
class HttpContent {
 public:
    HttpContent();
    ~HttpContent();

    void ParseLine(Buffer* buffer);
    bool ParseContent(Buffer* buffer);
    bool GetCompleteRequest() { return parse_state_ == kParseGotCompleteRequest; }

    const HttpRequest& request() const { return request_; }
    void ResetContentState() {
        parse_state_ = kParseRequestLine;
        line_state_  = kLineOK; 
    }
    void show() {
        printf("Method:%d, Url:%s, Version:%d\n", request_.method(), request_.path().c_str(), request_.version());
    }

 private:
    int checked_idx_;
    HttpRequest request_;                       // http的请求
    HttpRequestParseState parse_state_;         // 主状态机
    HttpRequestParseLine line_state_;           // 从状态机
};

}   // namespace bing

#endif