#include "Learn-Muduo/Net/Channel.h"
#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Base/TimeStamp.h"
#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>

bing::EventLoop* G_loop;

using namespace bing;

void timeout(TimeStamp receiveTime) {
    printf("%s Timeout !\n", receiveTime.toString().c_str());
    G_loop->quit();
}


int main()
{

    printf("%s started\n", TimeStamp::now().toString().c_str());
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
