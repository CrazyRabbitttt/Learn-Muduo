#ifndef BING_NET_SOCKET_H
#define BING_NET_SOCKET_H

namespace bing {

class Socket {
public:
    explicit Socket(int sockfd)
        :sockfd_(sockfd) {}
    
    ~Socket();

    int fd() const { return sockfd_; }


private:
    const int sockfd_;      //file descripter 
};


}  //namespace bing

#endif