#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Base/Thread.h"
#include "stdio.h"

using namespace bing;
using namespace bing::currentThread;


bing::EventLoop* G_loop;


void ThreadFunc1() {

    printf("threadFunc(): pid = %d, tid = %d\n", 
    getpid(), bing::currentThread::tid());

    bing::EventLoop loop;
    loop.loop();            //just polling for five seconds 
}

void ThreadFunc2() {
    G_loop->loop();         //run loop but not in this thread 
}

int main()
{
    // printf("threadFunc(): pid = %d, tid = %d\n", 
    // getpid(), bing::currentThread::tid());


    // bing::EventLoop loop;

    // bing::Thread thread(ThreadFunc1);
    // thread.start();


    // loop.loop();

    bing::EventLoop loop;
    G_loop = &loop;

    bing::Thread thread(ThreadFunc2);
    thread.start();

    thread.join();


}

