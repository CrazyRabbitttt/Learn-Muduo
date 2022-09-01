#ifndef BING_NET_EPOLLER_H
#define BING_NET_EPOLLER_H
#include "Poller.h"
#include <sys/epoll.h>

namespace bing {

/*
    Epoller()                   -> epoll_create
    updatechannel()             -> EPOLL_CLT_ADD, EPOLL_CLT_MOD
    removeChannel()             -> EPOLL_CTL_DEL
        Poll()                  -> epoll_wait获得可以进行XX的事件
*/

class Epoller : public Poller{
public:
    Epoller(EventLoop* loop);
    ~Epoller() override;

    //epoll的创建、 更新、 删除, poll就是进行wait
    TimeStamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:    
    static const int kEventListSize = 16;   //创建epoll的初始化的事件数目
    using EventList = std::vector<epoll_event>;

    void update(int operation, Channel* channel);

    //填充活跃的连接，让EventLoop啥的知道
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    int epollfd_;                           //epoll需要一个文件描述符号作为参数
    EventList events_;                      //事件表（包含了关注的事件和用户的数据）
};

}   //anmespace bing

#endif