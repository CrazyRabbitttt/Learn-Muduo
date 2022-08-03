#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/TimeStamp.h"
#include <iostream>

using namespace bing;


void func1() {
    printf("pid = %d, tid = %d\n", getpid(), bing::currentThread::tid());
    printf("now %s\n", bing::TimeStamp::now().toString().c_str());
}


int main()
{   

    std::string currentName = "Current Name\n";

    // using ThreadFunc = std::function<void()>;

    ThreadFunc func = []() { printf("The func() is running ...\n");} ;

    Thread curThread(func1, currentName);


    curThread.start();


}
