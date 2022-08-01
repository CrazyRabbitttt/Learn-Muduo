#ifndef BING_NET_POLLER_H
#define BING_NET_POLLER_H

#include <vector>
#include <unordered_map>
#include "Learn-Muduo/Base/nocopyable.h"

namespace bing {


class Channel;
class EventLoop;

class Poller : nocopyable {
public:
    Poller(EventLoop* loop);
    ~Poller();

protected:
    //socket fd -> Channel*
    using ChannelMap = std::unordered_map<int, Channel*>;   
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;      //Pollwer 属于哪个EventLoop事件循环
};

}   //namespace bing 

#endif 