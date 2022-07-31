#include "Learn-Muduo/Base/Thread.h"
#include <iostream>
using namespace bing;
int main()
{   

    std::string currentName = "Current Name\n";

    // using ThreadFunc = std::function<void()>;

    ThreadFunc func = []() { printf("The func() is running ...\n");} ;


    Thread curThread(func, currentName);


    curThread.start();




    


}
