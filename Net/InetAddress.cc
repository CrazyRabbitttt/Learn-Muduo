#include "Learn-Muduo/Net/InetAddress.h"
#include <string.h>
#include <iostream>

using namespace bing;

InetAddress::InetAddress(uint16_t port, std::string ip) {
    bzero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);

    //covert ip from string to binary 
    //         mode     src                dst 
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr);
}

std::string InetAddress::toIp() const { 
    char buf[64];
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);    
    return buf;
}

std::string InetAddress::toIpPort() const {
    //format:   ip:port 
    char buf[64];
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof (buf));
    size_t len = strlen(buf);
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf + len, ":%u", port);
    return buf;
}

uint16_t InetAddress::port() const {
    return ntohs(addr_.sin_port);
}



