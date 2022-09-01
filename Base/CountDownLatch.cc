#include "CountDownLatch.h"


using namespace bing;

CountDownLatch::CountDownLatch(int count) 
    :mutex_(), condition_(mutex_), count_(count)
    {}


//等待计数器为0
void CountDownLatch::wait() {
    MutexLockGuard lock(mutex_);
    while (count_ > 0) {
        condition_.wait();
    }
}

//计数器减1， 如果为0， 就广播
void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    count_--;
    if (count_ == 0) {
        condition_.notifyAll();
    }
}

int CountDownLatch::getCount() {
    MutexLockGuard lock(mutex_);
    return count_;
}

