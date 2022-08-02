#ifndef BING_NET_POLLER_H
#define BING_NET_POLLER_H

#include <vector>
#include <unordered_map>
#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/TimeStamp.h"
#include "poll.h"

namespace bing {


class Channel;
class EventLoop;

class Poller : nocopyable {
public:
    using ChannelList = std::vector<Channel*>;
    
    Poller(EventLoop* loop);
    ~Poller();

    //进行事件的注册
    TimeStamp poll(int timeouts, ChannelList* activeChannels);

    void updateChannel(Channel* channel);

private:

    //进行活跃fd的注册
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    //socket fd -> Channel*
    using ChannelMap = std::unordered_map<int, Channel*>;   
    using PollfdList = std::vector<struct pollfd>;

    ChannelMap channels_;
    PollfdList pollfds_;
    EventLoop* ownerLoop_;      //Pollwer 属于哪个EventLoop事件循环

};

}   //namespace bing 

#endif 