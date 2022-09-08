#include "tuunel.h"
#include "Cache.h"
#include "lovebing.h"

#include <iostream>
#include <string.h>
#include <algorithm>
#include <string>
#include <map>

#define READER_SAME_TIME  5

InetAddress* g_serverAddr;                   
EventLoop* g_eventLoop;
std::map<std::string, TunnelPtr> g_tunnels;

static const char http[]       = "HTTP/1.";
static const char resStatus[]  = "HTTP/1.0 200 OK\r\n";
static const char resConn[]    = "Connection: close\r\n";
static const char rescrlf[]    = "\r\n"; 

static const char resError[]   = "HTTP/1.0 404 Not Found\r\n";

// Buffer* tmpbuffer;
// int cnt = 0;


enum Parsestate { kParseRequest, kParseHeader, };

enum Method{ kGet, kOther};   
enum Version {
    kUnknown,
    kHttp10,
    kHttp11,
};   

static int readcnt;         // 读者的数量
static sem_t mutex, W;      // 保护读者数据 & 写者
static cache_t cache;
static int CacheSize;       // cache size 
static Method method_;      // the httpMethod 
static std::string path_;                                  // urlpath
static std::string query_;
static Version version_;


using namespace muduo;

bool ParseRequestMethod(const char* start, const char* end) {
    string method(start, end); 
    if (method == "GET") {
        method_ = kGet;
    } else method_ = kOther;
    return method_ == kGet;
}

bool ParseRequestLine(const char* start, const char* end) {
  const char* space = nullptr;
  space = std::find(start, end, ' ');
  if (space == end) {
    return false;
  }

    // GET /root/hello.html? HTTP/1.1
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


int  ParseRequest(Buffer* buffer) {
    printf("in parse request\n");
    bool parseOk  = true;                           // 解析一行的状态
    Parsestate parsestate = kParseRequest;
    bool lineMore = true;
    printf("%s", buffer->peek());
    while (lineMore) {
        if (parsestate == kParseRequest) {
            const char* crlf = buffer->findCRLF();
            if (crlf) {
                 const char* colon = std::find(buffer->peek(), crlf, 'H');
                 if (colon == crlf) {       // 空行
                    lineMore = false;
                    printf("读到空行了\n");
                    return 2;
                 }
                 parseOk = ParseRequestLine(buffer->peek(), crlf);      // 解析头部
                 if (parseOk) {
                    printf("Parse line ok.\n");
                    // parsestate = kParseHeader;
                    buffer->retrieveAll();
                    lineMore = false;
                 } else lineMore = false;
            } else {
                lineMore = false;        
                parseOk = false;                 
            }
        } else if (parsestate == kParseHeader) {
            printf("Parse header...\n");
            const char* crlf = buffer->findCRLF();
            if (crlf) {
                const char* colon = std::find(buffer->peek(), crlf, ':');
                if (colon == crlf) {                                    // 一定是会有空行的
                    lineMore = false;
                } 
            } else {
                lineMore = false;
            }
        }
    }
    return parseOk;
}


// 读模型， 从Cache进行访问, 从Cache中找有没有缓存的数据
obj_t* readItem(const char* targetURI) {
    printf("here3??\n");
    P(&mutex);
    readcnt++;                   // 读者的数量++
    if (readcnt == 1) {          // 第一个读者，拿写锁，不让写操作进行 
        P(&W);
    }   
    V(&mutex);
    printf("here4??\n");
    /********  reading section ************/
    obj_t* cur = cache.head->next;
    printf("here5??\n");
    putchar(cur->flag);
    while (cur->flag != '@') {
        printf("%s:%s", targetURI, cur->uri);
        if (strcmp(targetURI, cur->uri) == 0) {
            return cur;
        } 
        cur = cur->next;
    }
    printf("here6??\n");
    /********  reading section ************/

    P(&mutex);
    readcnt--;
    if (readcnt == 0) {         // 全部的读都没了，放开写锁
        V(&W);
    }
    V(&mutex);  
    printf("if is ok?\n");

    return nullptr;
}


// 写模型，将obj写到cache中，可能饥饿
void writeToCache(obj_t* obj) {
    // 如果current cap, if full, delete one 
    while (obj->respBodyLen + CacheSize > MAX_CACHE_SIZE && cache.head->next != cache.tail) {
        obj_t* last = cache.tail->prev;
        last->next->prev = last->prev;
        last->prev->next = last->next;

        last->next = NULL;
        last->prev = NULL;
        free(last);
    }

    // 将obj放进cache中，最前面的是最常访问的
    obj->next = cache.head->next;
    obj->prev = cache.head;
    cache.head->next->prev = obj;
    cache.head->next       = obj;
    CacheSize += obj->respBodyLen;
}



// Proxy收到连接请求
void onServerConnection(const TcpConnectionPtr& conn) {
    // printf("New Connection from %s\n", conn->peerAddress().toIpPort().c_str());
    if (conn->connected()) {
        conn->setTcpNoDelay(true);
        conn->stopRead();               // 连接建立了，不用监听socket的读事件了
        TunnelPtr tunnel(new Tunnel(g_eventLoop, *g_serverAddr, conn));
        tunnel->setup();
        tunnel->connection();           // proxy同服务器进行连接
        g_tunnels[conn->name()] = tunnel;
    } else {
        assert(g_tunnels.find(conn->name()) != g_tunnels.end());
        g_tunnels[conn->name()]->disconnection();           // 断开同Server的链接
        g_tunnels.erase(conn->name());
    }
}



// Proxy 接收到客户端的信息
void onServerMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp) {
    if (!conn->getContext().empty()) {              // 也就是有连接？
        const TcpConnectionPtr& clientConn = boost::any_cast<const TcpConnectionPtr&>(conn->getContext());
        // buffer 是接收的缓冲
        int res = ParseRequest(buffer);
        if (res == 1) {              // 如果是合法的GET请求
            // 不管，等到读取到\r\n之后再一起处理，这时候已经将uri存储下来了
        } else if (res == 2){
            // obj_t* obj = (obj_t*)malloc(sizeof(*obj));              // 线程私有？？可重入
            obj_t* obj;
            obj = readItem(path_.c_str());
            if (obj) {          // 如果是找到了缓存的内容
                printf("Hit Cache, Return to Client\n");
                // 发送Status, 
                conn->send(resStatus);
                conn->send(resConn);
                conn->send(rescrlf);
                conn->send(obj->respBody);
                conn->shutdown();                                   // 关闭掉同客户端的连接
            } else {
                printf("在Cache中没找到, 向Server发起请求:\n");
                string tmp = "GET "; tmp += path_; 
                if (version_ == kHttp10) tmp += " HTTP/1.0\r\n";
                else tmp += " HTTP/1.1\r\n";
                clientConn->send(tmp.c_str());
                clientConn->send(rescrlf);
                printf("send message to server\n");
            }
            // free(obj);
        } else {
            printf("请求压根就不合法,直接返回错误得了\n");
            conn->send(resError);
            conn->send(resConn);
            conn->send(rescrlf);
            // conn->shutdown();
        }
    }
}


int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: %s <ip> <port> <listenport>\n", argv[0]);
        return 0;
    }
    const char* Ip = argv[1];                                      // Server Ip
    uint16_t port  = static_cast<uint16_t>(atoi(argv[2]));        // Server Port 
    InetAddress serverAddr(port, Ip);
    g_serverAddr = &serverAddr;

    uint16_t proxyPort = static_cast<uint16_t>(atoi(argv[3]));
    InetAddress proxyAddr(proxyPort);

    EventLoop loop;                 // Proxy的EventLoop
    g_eventLoop = &loop;

    TcpServer server(&loop, proxyAddr, "Proxy");

//=====================================================================================================
    // 进行读写锁信号量的初始化
    Sem_init(&mutex, 0, 5);             // 对cache的访问权限,多读可同时
    Sem_init(&W    , 0, 1);             // 写的权限，互斥访问
    readcnt = 0;

    CacheSize = 0;                      // 缓存了的len的大小
    initializeCache(&cache);
    obj_t* tmpobj = (obj_t*)malloc(sizeof(*tmpobj));
    tmpobj->flag = '$';
    *tmpobj->respBody   = 0;
    *tmpobj->respHeader = 0;

    char body[] = "This is the test body";
    char header[] = "Host: localhost";
    strcat(tmpobj->respBody, body);
    strcat(tmpobj->respHeader, header);
    tmpobj->respBodyLen += strlen(body);
    tmpobj->respHeaderLen += strlen(header);
    // strcat(tmpobj->uri, "/Test/index.html");
    tmpobj->uri = "/haha";
    writeToCache(tmpobj);
    // free(tmpobj);
//=====================================================================================================
    server.setConnectionCallback(onServerConnection);
    server.setMessageCallback(onServerMessage);

    server.start();

    loop.loop();
    free(tmpobj);
}
