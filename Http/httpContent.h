#ifndef BING_HTTPCONTENT_H
#define BING_HTTPCONTENT_H

#include "httpRequest.h"
#include "httpResponse.h"
#include <string.h>

namespace bing {

// 主状态机的操控
class HttpContent {
 public:
    HttpContent();
    ~HttpContent();

    bool ParseContent(Buffer* buffer);
    bool GetCompleteRequest() { return parse_state_ == kParseGotCompleteRequest; }

    const HttpRequest& request() const { return request_; }
    void ResetContentState() {
        parse_state_ = kParseRequestLine;
    }

 private:
    int checked_idx_;
    HttpRequest request_;                       // http的请求
    HttpRequestParseState parse_state_;         // 主状态机
};

}   // namespace bing

#endif