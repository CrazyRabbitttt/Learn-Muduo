#ifndef BING_HTTP_PARSESTATE_H
#define BING_HTTP_PARSESTATE_H

namespace bing {

// 主状态机的状态：请求行、头部
enum HttpRequestParseState {
    kParseRequestLine,      
    kParseRequestHeader,
    kParseBody,
    kParseGotCompleteRequest,
    kParseErrno,
};

// 从状态机的状态：一行解析成功、还要更多、解析错误（\r, \n提前出现等）, 新版本不要从状态机了

}   // namespace bing 

#endif