#include "Learn-Muduo/Net/Poller.h"
#include "Learn-Muduo/Net/Channel.h"

using namespace bing;

Poller::Poller(EventLoop* loop)
    :ownerLoop_(loop) 
    {}

bool Poller::hasChannel(Channel* channel) const {
    //是否是在channels_中
    auto it = channels_.find(channel->fd());
    //fd -> channel
    //能够在map中找到并且channel确实是这个·
    return it != channels_.end() && it->second == channel;
}