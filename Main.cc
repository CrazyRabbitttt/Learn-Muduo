#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/TcpServer.h"
#include "Learn-Muduo/Net/InetAddress.h"
#include "Learn-Muduo/Http/httpServer.h"
#include "Learn-Muduo/Http/httpSourceFile.h"

#include <string>

using namespace bing;


void HttpResponseCallBack(const HttpRequest& request, HttpResponse& response) {
    // 进行资源的分发处理
    if (request.method() != kGet) {
        response.setStatusCode(k400BadRequest);
        response.setStatusMessage("Bas Request");
        response.setCloseConnection(true);
        return;
    }

    const string& path = request.path();
    if (path == "/") {
        response.setStatusCode(k200OK);
        response.setBodyType("text/html");
        response.setBody(Website);
    } else {
        printf("The Path is not /\n");
    }
}


int main(int argc, char** argv) {
    // default threadsnum : 4, port : 80;
    // int threadNum = 4, Port = 80;
    // std::string logPath = "./WebServer.log";

    // int opt;
    // const char* str = "t:l:p";  // thread num, log path, port num 
    // while ((opt = getopt(argc, argv, str)) != -1) {
    //     switch (opt) {
    //         case 't': {
    //             threadNum = atoi(optarg);
    //             break;
    //         }
    //         case 'l': {
    //             logPath = optarg;
    //             if (logPath.size() < 2 || optarg[0] != '/') {
    //                 printf("LogPath 应该以 \"/\"开头\n");
    //                 abort();
    //             }
    //             break;
    //         }
    //         case 'p' : {
    //             Port = atoi(optarg);
    //             break;
    //         }
    //         default:
    //             break;
    //     }
    // }
    if (argc <= 1) {
        printf("Usage: %s portname\n", argv[0]);
        return 0;
    }

    EventLoop mainloop;
    InetAddress addr(atoi(argv[1]));     // default 127.0.0.1
    HttpServer server(&mainloop, addr);
    server.setHttpResponseCallback(HttpResponseCallBack);
    server.start();
    mainloop.loop();   

}


