#ifndef BING_BASE_COUNTDOWNLATCH_H
#define BING_BASE_COUNTDOWNLATCH_H

#include "Learn-Muduo/Base/Condition.h"
#include "Learn-Muduo/Base/Mutex.h"
#include "Learn-Muduo/Base/nocopyable.h"

namespace bing {


class CountDownLatch : nocopyable
{
 public: 
    explicit CountDownLatch(int count);     
    
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