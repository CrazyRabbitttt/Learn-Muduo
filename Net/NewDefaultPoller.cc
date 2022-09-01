#include "Poller.h"
#include "Epoller.h"

using namespace bing;

//创建默认的Poller : Epoll
Poller* Poller::newDefaultPoller(EventLoop* loop) {
    //不会使用poll的，这里只是使用了epoll
    if (::getenv("USE_POLL")) {
        return nullptr; 
    } else {
        return new Epoller(loop);
        // return epoller;         //默认的继承是private? ,这里改为public就可以了
    }
}
