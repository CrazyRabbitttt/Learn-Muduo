#include "./CountDownLatch.h"

using namespace Muduo;

CountDownLatch::CountDownLatch(int count) 
    :mutex_(), condition_(mutex_), count_(count)
    {}

void CountDownLatch::wait() {
    //等待⌛️计数器为0
    MutexLockGuard lock(mutex_);
    while (count_ > 0) {        //防止虚假唤醒
        condition_.wait();
    }
}

void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    count_--;
    if (count_ == 0) {
        condition_.notifyAll();
    }
}

int CountDownLatch::getCount()  {
    MutexLockGuard lock(mutex_);
    return count_;
}

