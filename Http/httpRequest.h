#ifndef BING_HTTP_REQUEST_H
#define BING_HTTP_REQUEST_H

#include "Learn-Muduo/Http/httpParseState.h"
#include <string>
#include <map>

using std::string;

namespace bing {

static const char http[] = "HTTP/1.";

enum Method {
    kGet, 
    kPost,
    kPut,
    kDelete,
};

enum Version {
    kUnknown,
    kHttp10,
    kHttp11,
};

class HttpRequest {
 public:
    HttpRequest() = default;
    ~HttpRequest() = default;

    bool ParseRequestMethod(const char* start, const char* end);

    void ParseRequestLine(const char* start, const char* end, 
                        HttpRequestParseState& state);

    void ParseHeaders(const char* start, const char* end,
                        HttpRequestParseState& state);
    
    void ParseBody(const char* start, const char* end, 
                        HttpRequestParseState& state); 

    Method method() const { return method_; }
    const string& path() const { return path_; }
    const string& query() const { return query_; }
    Version version() const { return version_; }
    const std::map<string, string>& headers() const { return hearders_; }

    string GetHeader(const string& header) const {
        auto item = hearders_.find(header);
        if (item == hearders_.end()) {
            return string();
        } else {
            return item->second;
        }
    }

 private:
    Method method_;
    Version version_;

    std::string path_;                                  // urlpath
    std::string query_;
    std::map<std::string, std::string> hearders_;       // hearders
};


}   // namespace bing 

#endif