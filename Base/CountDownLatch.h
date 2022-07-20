#ifndef MUDUO_BASE_COUNTDOWNLATCH_H_
#define MUDUO_BASE_COUNTDOWNLATCH_H_

#include "./Condition.h"
#include "./Mutex.h"
#include "./Nocopyable.h"
namespace Muduo{

class CountDownLatch : nocopyable 
{
public:
    explicit CountDownLatch(int count);

    void wait();                //等待计时器count == 0
        
    void countDown();           //计时器--

    int  getCount(); 

private:
    MutexLock mutex_;
    Condition condition_;
    int count_;
};

}




#endif