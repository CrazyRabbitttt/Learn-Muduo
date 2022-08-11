#include "Learn-Muduo/Net/Epoller.h"
#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/EventLoop.h"

#include <error.h>
#include <string.h>

using namespace bing;


const int kNew     = -1;    //没有加入到epoll中
const int kAdded   =  1;    //已经添加到epoll中去了
const int kDeleted =  2;    //已经从epoll中删除了


Epoller::Epoller(EventLoop* loop) 
    :Poller(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),       //关掉了的话父进程的数据是不会关闭的
    events_(kEventListSize)                         //vector, 16个空的数据
    {
        if (epollfd_ < 0) {
            printf("Epoller::Epoller() error : %s\n", strerror(errno));
        }
    }

//关闭文件描述符号
Epoller::~Epoller() {
    ::close(epollfd_);
}    


//epoll_wait, 将发生的事件填入activeChannels
TimeStamp Epoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_,
                                 &(*events_.begin()),
                                 static_cast<int>(events_.size()),
                                 timeoutMs);
    
    TimeStamp now(TimeStamp::now());
    if (numEvents > 0) {
        //TODO: log 
        printf("%d events happend\n", numEvents);
        //将发生的事件填充进activeChannel中去
        fillActiveChannels(numEvents, activeChannels);

        //将事件表进行扩容
        if (events_.size() == numEvents) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        //do nothing 
        printf("nothing happend\n");
    } else {
        if (errno != EINTR) {
            printf("EPoller::poll() : epoll_wait error\n");
        }
    }
    return now;
}

//将发生的事件填充到活跃Channels中去
void Epoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    //activeChannels是vector
    for (int i = 0; i < numEvents; ++i) {
        //ptr就是与fd相关的数据, 拿到发生事件的Channel
        Channel* nowchannel = static_cast<Channel*>(events_[i].data.ptr);
        nowchannel->set_revents(events_[i].events); //将事件的状态进行填充
        activeChannels->push_back(nowchannel);
    }
}

//事件进行改变的顺序
// Channel update remove => EvnentLoop updateChannel removeChannel => Poller updateChannel removeChannel

void Epoller::updateChannel(Channel* channel) {
    printf("到达epoll的注册事件\n");
    const int index = channel->index();
    //进行添加, 添加到epoll中去
    if (index == kNew || index == kDeleted) {
        //1. 添加到channels中，map(fd,channel)
        int fd = channel->fd();
        channels_[fd] = channel;

        //2. 设置为已经添加了
        channel->set_index(kAdded);

        //3.添加到epoll中
        update(EPOLL_CTL_ADD, channel);
    } else {        //kAdded, 已经添加过了, 想要进行修改
        // 对所有的事件都是不感兴趣了， 删除掉
        if (channel->isNonEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

//将fd从epoll中移除，不关心这个文件描述符号了
void Epoller::removeChannel(Channel* channel) {
    //接下来就是将这个fd毫不关心了
    int fd = channel->fd();
    channels_.erase(fd);

    int index = channel->index();
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}



//进行实际上的epoll 上的调用
void Epoller::update(int operation, Channel* channel) {
    struct epoll_event event;
    bzero(&event, sizeof event);

    //添加或者是修改？ 

    int modfd = channel->fd();
    printf("需要注册的fd:%d\n", modfd);
    //绑定fd 和 fd相关的channel, 事件可读的时候能够拿到channel了
    event.data.fd  = modfd;
    event.data.ptr = channel;

    //更新事件
    event.events   = channel->events();     //关注的事件是什么

    if (::epoll_ctl(epollfd_, operation, modfd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            printf("epoll_ctl op = del error, fd = %d\n", modfd);
        } else {
            printf("epoll_ctl op = add/mod error, fd = %d\n", modfd);
        }
    }
    printf("注册完成\n");
}








