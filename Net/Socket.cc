#include "Learn-Muduo/Net/Socket.h"
#include <unistd.h>

using namespace bing;

Socket::~Socket() {
    close(sockfd_);
}