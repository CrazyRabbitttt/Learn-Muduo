#include "Learn-Muduo/Base/CountDownLatch.h"
#include <iostream>
#include <unistd.h>
using namespace bing;

CountDownLatch latch(1);

void tongbu() {
    sleep(1);
    printf("lalalala\n");    
    latch.countDown();
}


int main()
{
    printf("running ...\n");

    tongbu();
    latch.wait();
    printf("end, after count == 0");    
    return 0;
}