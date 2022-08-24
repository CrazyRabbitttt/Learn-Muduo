#include "Learn-Muduo/Http/httpRequest.h"
#include <algorithm>

using namespace bing;

bool HttpRequest::ParseRequestMethod(const char* start, const char* end) {
    string method(start, end);
    bool has_method = true; 
    if (method == "GET") {
    method_ = kGet;
  } else if (method == "POST") {
    method_ = kPost;
  } else if (method == "PUT") {
    method_ = kPut;
  } else if (method == "DELETE") {
    method_ = kDelete;
  } else {
    has_method = false;
  }
  return has_method;
}


// GET /url/xxxx/xx.html HTTP/1.1
void HttpRequest::ParseRequestLine(const char* start, const char* end, HttpRequestParseState& state)  {
  // 解析Method, [start, end) !!!!!!!!!!!!!!!!!!!!!!!!!!!!![start, end)
  const char* space1 = std::find(start, end, ' ');
  if (space1 == end) {   
    state = kParseErrno;
    return;
  }
  if (!ParseRequestMethod(start, space1)) {    // 获得Method
    state = kParseErrno;
    return ;
  }
  start = space1 + 1;                         

//==========================================================================================================================
  // 下面解析url可能出现url?query Version 
  const char* space = std::find(start, end, ' ');
  if (space == end) {
    state == kParseErrno;
    return ;
  }

  const char* wenhao = std::find(start, end, '?');
  if (wenhao == end) {         
    path_ = std::move(string(start, space));    // move, 减少了一次拷贝行为
  } else {
    path_ = std::move(string(start, wenhao));
    query_ = std::move(string(wenhao + 1, space));
  }
  start = space + 1;

//==========================================================================================================================
  // if has subsequence : HTTP/1.    if has, Return the location of first matched element 
  const int httpLen = sizeof(http) / sizeof(char) - 1;
  const char* httpindex = std::search(start, end, http, http + httpLen);
  if (httpindex == end) {
    state = kParseErrno;
    return;
  } 
  
  // 判断一下版本
  const char chr = *(httpindex + httpLen);
  if (httpindex + httpLen + 1 == end && (chr == '1' || chr == '0')) {
    if (chr == '0') {
      version_ = kHttp10;
    } else {
      version_ = kHttp11;
    }
  } else {
    state == kParseErrno;
    return;
  }
  printf("Parse Request Line, Method:%d, Url:%s, Version:%d\n", method_, path_.c_str(), version_);
//============
  state = kParseRequestHeader;            // 下面主状态机去解析headers去吧
}
                     
void HttpRequest::ParseHeaders(const char* start, const char* end, HttpRequestParseState& state ) {
    // 如果是头部的全都解析完毕， 只剩下空的➕\r\n
    if (start == end && *start == '\r' && *(start + 1) == '\n') {
      state = kParseGotCompleteRequest;
      return ;
    }

    // name:value\r\n
    const char* colon = std::find(start, end, ':');
    if (colon == end) {
      state = kParseErrno;
      return;
    } 
      // hearders_[std::move(string(start, colon))] = 

    const char* edge = colon + 1;
    while (*(edge++) != ' ') {}         // 直到遇见' '为止
    hearders_[std::move(string(start, colon))] = std::move(string(colon + 1, edge));
    return ;
}


void  HttpRequest::ParseBody(const char* start, const char* end,
                          HttpRequestParseState& state) {}