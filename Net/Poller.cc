#include "Learn-Muduo/Net/Poller.h"
#include "Learn-Muduo/Net/Channel.h"


#include <assert.h>

using namespace bing;

Poller::Poller(EventLoop* loop) :  ownerLoop_(loop) {}

Poller::~Poller() {}


TimeStamp Poller::poll(int times, ChannelList* activeChannels) {
    //捕获activeFd -> 回送到ActiveChannels中
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), times);

    TimeStamp now(TimeStamp::now());    

    if (numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);      //进行活跃的Channels的填充，注册
    } else if (numEvents == 0) {
        printf("SYS:  Poller::poll(), Nothing happend\n");
    } else {
        printf("WARN: Poller::poll() error!!!!\n");
    }
    return now;
}

//遍历pollfds, 将fd对应的Channel注册到activeChannel中
void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    for (PollfdList::const_iterator pfd = pollfds_.begin(); 
        pfd != pollfds_.end() && numEvents > 0; ++pfd)
    {
        if (pfd->revents > 0) {
            --numEvents;
            ChannelMap::const_iterator fd1 = channels_.find(pfd->fd);
            Channel* channel = fd1->second;
            channel->set_revents(pfd->revents);         //保存当前活跃的事件。供handleEvent
            activeChannels->push_back(channel);
        }
    }
}


void Poller::updateChannel(Channel* channel) {
    if (channel->index() < 0) {     //不存在，新添加
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        pollfds_.push_back(pfd);
        int idx = static_cast<int>(channels_.size()) - 1;
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    } else {                        //更新
        int idx = channel->index();
        struct pollfd& pfd = pollfds_[idx];
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        if (channel->isNonEvent()) {
            pfd.fd = -1;
        }
    }
}



