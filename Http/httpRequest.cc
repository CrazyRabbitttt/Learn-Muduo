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

bool HttpRequest::ParseRequestLine(const char* start, const char* end) {
  const char* space = nullptr;
  space = std::find(start, end, ' ');
  if (space == end) {
    return false;
  }

  // 解析Method
  if (!ParseRequestMethod(start, space)) {
    return false;
  }

  start = space + 1;
  space = std::find(space + 1, end, ' ');
  if (space == end) {
    return false;
  }
  const char* query_ptr = std::find(start, end, '?');
  if (query_ptr != end) {
    path_.assign(start, query_ptr);
    query_.assign(query_ptr + 1, space);
  } else {
    path_.assign(start, space);
  }
  start = space + 1;
  // 解析HTTP
  bool parseHttp = (start + 8 == end) && std::equal(start, end - 1, http);
  if (!parseHttp || (*(end - 1) != '0' && *(end - 1) != '1')) {
    version_ = kUnknown;
    return false;
  }

  if (*(end - 1) == '0') {
    version_ = kHttp10;
  } else {  
    version_ = kHttp11;
  } 

  return true;
}

bool HttpRequest::ParseHeaders(const char* start, const char* colon, const char* end) {
  // 传过来肯定是合法的？
  const char* validstart = colon + 1;
  while (*validstart == ' ') ++validstart;
  hearders_[std::move(string(start, colon))] = std::move(string(validstart, end));
  return true;
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