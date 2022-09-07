#include "EventLoop.h"
#include "TcpServer.h"
#include "InetAddress.h"
#include "httpServer.h"
#include "httpSourceFile.h"
#include "asynclogging.h"
#include "logger.h"

#include <memory>

using namespace bing;

// std::unique_ptr<AsyncLogging> asyncLog;     // 异步日志的前后端？

// extern void setOutputFunc(Logger::OutputFunc);

// void asyncOutputFunc(const LogStream::Buffer& buffer) {
//     asyncLog->append(buffer.data(), buffer.len());
// }

void HttpResponseCallBack(const HttpRequest& request, HttpResponse& response) {
    // 进行资源的分发处理
    if (request.method() != kGet) {
        response.setStatusCode(k400BadRequest);
        response.setStatusMessage("Bad Request");
        response.setCloseConnection(true);
        return;
    }

    const string& path = request.path();
    if (path == "/") {
        response.setStatusCode(k200OK);
        response.setBodyType("text/html");
        response.setBody(Website);
    } else {
        response.setStatusCode(k404NotFound);
        response.setBodyType("text/html");
        response.setBody("Not Found!!!!");
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
    std::string tmp("");
    if (argc <= 1) {
        printf("Usage: %s portname [logpath]\n", argv[0]);
        return 0;
    } else if (argc == 2) {
        printf("Use the default logPath as you didn't  set it.\n");
    } else if (argc == 3){
        printf("argv1:");
        std::cout << argv[1];
        tmp = std::move(string(argv[2]));
    }
    
    // asyncLog.reset(new AsyncLogging(3, 2 * 1024 * 1024));       // 滚动时间3秒，滚动大小2M
    // asyncLog->setLogName(tmp);  
    // setOutputFunc(asyncOutputFunc);                             // 调用异步写线程类的append
    // asyncLog->start();                                          // 开启异步写的后端线程


    EventLoop mainloop;
    InetAddress addr(atoi(argv[1]));     // default 127.0.0.1
    HttpServer server(&mainloop, addr, true);
    server.setHttpResponseCallback(HttpResponseCallBack);
    server.start();
    mainloop.loop();   

}


