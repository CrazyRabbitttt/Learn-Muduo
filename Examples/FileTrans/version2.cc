#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

const char* g_file = NULL;
const int kBufferSize = 64*1024;        //每次发送64字节的数据

typedef std::shared_ptr<FILE> FIlePtr;

void onHighWaterMark(const TcpConnectionPtr& conn, size_t len)
{
  LOG_INFO << "HighWaterMark " << len;
}

void onConnection(const TcpConnectionPtr& conn) {
    LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort() << " -> "
           << conn->localAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected()) {
        LOG_INFO << "FileServer - Sending file " << g_file
             << " to " << conn->peerAddress().toIpPort();

        conn->setHighWaterMarkCallback(onHighWaterMark, kBufferSize+1);
        FILE* fp = ::fopen(g_file, "rb");

        if (fp) {
            // FIlePtr ctx(fp, ::fclose);          //删除的时候回调的函数
            //保存一下上下文（FILE*），下一次还用
            conn->setContext(fp);
            char buf[kBufferSize];
            size_t nread = ::fread(buf, 1, sizeof buf, fp);
            conn->send(buf, static_cast<int>(nread));
        } else {
            conn->shutdown();
            LOG_INFO << "FileServer no such file";
        }

    } else {
        if (!conn->getContext().empty()) {      //有上下文保存的数据，但是连接关闭了
            FILE* fp = boost::any_cast<FILE*>(conn->getContext());
            if (fp) {
                ::fclose(fp);
            }
        }
    }
}


void onWriteComplete(const TcpConnectionPtr& conn) {
    //继续进行读写
    FILE* fp = boost::any_cast<FILE*>(conn->getContext());
    char buf[kBufferSize];
    int nread = ::fread(buf, 1, sizeof buf, fp);
    if (nread) {
        conn->send(buf, nread);
    } else {
        ::fclose(fp);
        fp = NULL;
        conn->setContext(fp);
        conn->shutdown();
        LOG_INFO << "Server - done";
    }
}


int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid();
  if (argc > 1)
  {
    g_file = argv[1];

    EventLoop loop;
    InetAddress listenAddr(2021);
    TcpServer server(&loop, listenAddr, "FileServer");
    server.setConnectionCallback(onConnection);
    server.setWriteCompleteCallback(onWriteComplete);
    server.start();
    loop.loop();
  }
  else
  {
    fprintf(stderr, "Usage: %s file_for_downloading\n", argv[0]);
  }
}

