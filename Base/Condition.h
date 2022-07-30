#ifndef BING_BASE_CONDITION_H
#define BING_BASE_CONDITION_H

#include "Learn-Muduo/Base/Mutex.h"

#include <pthread.h>

namespace bing {
class Condition {
 public:
    explicit Condition(MutexLock& mutex)
        :mutex_(mutex)
    {
        pthread_cond_init(&cond_, NULL);
    }

    ~Condition() {
        pthread_cond_destroy(&cond_);
    }

    void notify() {
        pthread_cond_signal(&cond_);
    }

    void notifyAll() {
        pthread_cond_broadcast(&cond_);
    }

    void wait() {
        pthread_cond_wait(&cond_, mutex_.getPthreadLock());
    }

 private:
    pthread_cond_t cond_;
    MutexLock& mutex_;
};

}       //namespace bing

#endif