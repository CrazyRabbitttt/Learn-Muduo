#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/EventLoopThread.h"

#include <stdio.h>

void runInThread() {
    printf("runInthread():pid = %d, tid = %d\n", getpid(), bing::currentThread::tid());
}


int main()
{
    printf("main(): pid = %d, tid = %d\n", getpid(), bing::currentThread::tid());

    //new thread with one loop 
    bing::EventLoopThread loopThread;

    bing::EventLoop* loop = loopThread.startLoop();

    loop->runInLoop(runInThread);
    
    // sleep(1);

    // loop->runInLoop(runInThread);

    // sleep(2);

    // loop->quit();

    printf("exit main\n");

}




