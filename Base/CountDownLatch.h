#ifndef BING_BASE_COUNTDOWNLATCH_H
#define BING_BASE_COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"
#include "nocopyable.h"

namespace bing {


class CountDownLatch : nocopyable
{
 public: 
    explicit CountDownLatch(int count);     
    
    ~CountDownLatch() = default;
    //等待计数器为0
    void wait();

    void countDown();

    int getCount() ;

 private:
    MutexLock mutex_;
    Condition condition_;
    int count_;             //计数器
};




}   //namespace bing 
#endif