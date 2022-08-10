#include "Learn-Muduo/Net/EventLoop.h"
#include "Learn-Muduo/Net/EventLoopThread.h"
#include "Learn-Muduo/Base/Thread.h"
#include <stdio.h>


void func() {
    printf("runinThread(): pid = %d, tid = %d\n",
    getpid(), bing::currentThread::tid());
}

void runInthread() {
    func();
}


int main() {
    printf("main(): pid = %d, tid = %d\n",
         getpid(), bing::currentThread::tid());

    bing::EventLoopThread loopThread;
    bing::EventLoop* loop = loopThread.startLoop();

    loop->runInLoop(runInthread);
    sleep(1);

    loop->runInLoop(runInthread);

    sleep(2);

    loop->quit();

    printf("exit main\n");

    sleep(1);

    printf("loop still alive?\n");

}



// int main() {
//     bing::Thread thread(runInthread, "New Thread");
//     thread.start();
//     thread.join();
//     printf("Main: ");
//     func();
// }











