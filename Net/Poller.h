#ifndef BING_NET_POLLER_H
#define BING_NET_POLLER_H

#include <vector>
#include <unordered_map>
#include "nocopyable.h"
#include "TimeStamp.h"


namespace bing {


class Channel;
class EventLoop;

class Poller : nocopyable {
public:
    using ChannelList = std::vector<Channel*>;          //channelList 
    
    Poller(EventLoop* loop);
    virtual ~Poller() = default;        //多态下子类调用自身的析构函数

    //IO复用的统一接口：进行事件的注册, 删除， 更新
    virtual TimeStamp poll(int timeouts, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;


    //Channel是否是在本Poller中
    bool hasChannel(Channel* channel) const;

    //EventLoop通过这个接口获得默认的Poller·
    static Poller* newDefaultPoller(EventLoop* loop);
    
protected:
    //socket fd -> Channel*
    using ChannelMap = std::unordered_map<int, Channel*>;   
    ChannelMap channels_;

private:

    EventLoop* ownerLoop_;      //Pollwer 属于哪个EventLoop事件循环

};

}   //namespace bing 

#endif 