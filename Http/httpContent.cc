#include "Learn-Muduo/Http/httpContent.h"
#include "Learn-Muduo/Http/httpRequest.h"
#include "Learn-Muduo/Http/httpResponse.h"
#include "Learn-Muduo/Net/Buffer.h"

using namespace bing;


HttpContent::HttpContent()
    : checked_idx_(0), 
      parse_state_(kParseRequestLine) {}

HttpContent::~HttpContent() {}


void HttpContent::ParseLine(Buffer* buffer) {
    line_state_ = kLinemore;            // init subStatusMachine 
    if (buffer->readableBytes() == 0) return ;
    int read_idx = buffer->readableBytes();
    const char* buf = buffer->beginread();
    // 将buffer中的可读区域当成是一个以buf开头的buf看
    for (; checked_idx_ < read_idx; checked_idx_ ++) {
        const char chr = buf[checked_idx_];
        if (chr == '\r') {
            if (checked_idx_ == read_idx - 1) continue;             // 最后一个刚好是\r, \n还没读过来呢
            if (buf[checked_idx_+1] == '\n') {
                checked_idx_  += 2;
                line_state_ = kLineOK;                              // 🐛：多写了一个等于号
            } else {
                line_state_ = kLineError;
            }
            return ;
        } else if (chr == '\n') {
            if (checked_idx_ > 0 && buf[checked_idx_-1] == '\r') {
                checked_idx_ += 1;
                line_state_ = kLineOK;
            } else {
                line_state_ = kLineError;
            }
            return ;
        } else {
            continue;
        }
    }
    return ;
}

bool HttpContent::ParseContent(Buffer* buffer) {
    while (parse_state_ != kParseErrno) {
        ParseLine(buffer);      // 进行解析一行出来，更新checkindex
        if (line_state_ == kLinemore || line_state_ == kLineError) {
            if (line_state_ == kLineError) {
                parse_state_ = kParseErrno;
                checked_idx_ = 0;
            }
            break;
        }

        const char* start = buffer->beginread();
        const char* end   = buffer->beginread() + checked_idx_ - 2;     // 不要\r\n, 目前指向\r, 右开，没关系

        if (parse_state_ == kParseRequestLine) {
            request_.ParseRequestLine(start, end, parse_state_);
        } else if (parse_state_ == kParseRequestHeader) {
            request_.ParseHeaders(start, end, parse_state_);
        } else if (parse_state_ == kParseBody) {
            request_.ParseBody(start, end, parse_state_);
        } else if (parse_state_ == kParseGotCompleteRequest) {
            break;          // 解析完成了，退出
        } 
        
        buffer->retrieveUntilIdx(start + checked_idx_);
        checked_idx_ = 0;
    }
    return parse_state_ != kParseErrno;     // 只要不是报错就好了
}







