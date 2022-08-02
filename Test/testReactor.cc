#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>

bing::EventLoop* G_loop;

void timeout() {
    printf("Timeout !\n");
    G_loop->quit();
}


int main()
{
    bing::EventLoop loop;
    G_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    bing::Channel channel(&loop, timerfd);

    channel.setReadCallBack(timeout);       //可读了就调用timeout
    channel.enableReading();

    struct itimerspec howlong;

    memset(&howlong, 0, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);    

}
