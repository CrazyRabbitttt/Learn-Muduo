#ifndef BING_BASE_CONDITION_H
#define BING_BASE_CONDITION_H

#include "Mutex.h"

#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>

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

    bool waitForSeconds(double seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        const int64_t kNanoSeconds = 1000000000;
        int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSeconds);

        abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSeconds);
        abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSeconds);

        return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getPthreadLock(), &abstime);
    }

 private:
    pthread_cond_t cond_;
    MutexLock& mutex_;
};

}       //namespace bing

#endif