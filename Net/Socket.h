#ifndef BING_NET_SOCKET_H
#define BING_NET_SOCKET_H

#include <sys/socket.h>
#include <stdio.h>


namespace bing {

class InetAddress;

class Socket {
public:
    explicit Socket(int sockfd)
        :sockfd_(sockfd) {
            printf("Socket::sockfd: %d\n", sockfd_);
        }
    
    ~Socket();

    int fd() const { return sockfd_; }

    //bind, 
    void bindAddr(const InetAddress& localaddr);     

    //listen
    void listen();
    
    void setNoblock();

    //accept
    //成功返回no-blockd fd（对应了通信的socket）
    //失败了就返回-1
    int accept(InetAddress* peeraddr);

    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void keepAlive(bool on);


    void shutDownWrite();
private:
    const int sockfd_;      //file descripter 
};


}  //namespace bing

#endif