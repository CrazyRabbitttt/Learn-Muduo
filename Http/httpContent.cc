#include "Learn-Muduo/Http/httpContent.h"
#include "Learn-Muduo/Http/httpRequest.h"
#include "Learn-Muduo/Http/httpResponse.h"
#include "Learn-Muduo/Net/Buffer.h"

using namespace bing;


HttpContent::HttpContent()
    : checked_idx_(0), 
      parse_state_(kParseRequestLine) {}

HttpContent::~HttpContent() {}


bool HttpContent::ParseContent(Buffer* buffer) {
    bool lineMore = true;       // 主状态机是否是失败的
    bool parseOk  = true;       // 解析一行的状态
    const char* crlf = nullptr;
    while (lineMore) {
        if (parse_state_ == kParseRequestLine) {
            // const char* crlf = buffer->
            crlf = buffer->findCRLF();
            if (crlf) {
                parseOk = request_.ParseRequestLine(buffer->peek(), crlf);
                if (parseOk) {
                    parse_state_ = kParseRequestHeader;
                } else {
                    lineMore = false;
                }
            } else {
                lineMore = false;
            }
            
        } else if (parse_state_ == kParseRequestHeader) {
            crlf = buffer->findCRLF();
            if (crlf) {
                const char* colon = std::find(buffer->peek(), crlf, ':');
                if (colon == crlf) {
                    parse_state_ = kParseGotCompleteRequest;            // 读到空行了，解析完成
                    lineMore = false;
                } else {
                    parseOk = request_.ParseHeaders(buffer->peek(), colon, crlf);
                    if (!parseOk) lineMore = false;
                }
            } else {
                lineMore = false;
            }
            
        } else if (parse_state_ == kParseGotCompleteRequest) {
            lineMore = false;
        } else if (parse_state_ == kParseBody) {
            // FIX ME, Method POST
        }

        // 如果是对于一行判断成功了，更新一下buffer 中的指针的位置
        if (crlf) {
            buffer->retrieveUntilIdx(crlf + 2);
        }
    }

    return parseOk;
}



