#ifndef MUDUO_BASE_MUTEX_H
#define MUDUO_BASE_MUTEX_H
#include <assert.h>
#include <pthread.h>
#include "./Nocopyable.h"

namespace Muduo{

class MutexLock : public nocopyable
{
public:

    MutexLock() : holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock() {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_);
    }

    void lock() {
        pthread_mutex_lock(&mutex_);
    }

    void unlock() {
        // holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }

    //获得RAII维护的内容
    pthread_mutex_t* getPthreadMutex() {
        return &mutex_;
    }


private:
    pthread_mutex_t mutex_;
    pid_t holder_;
};



//RAII, use in stack 
class MutexLockGuard : public nocopyable
{
public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex)
    {
        mutex_.lock();      
    }

    ~MutexLockGuard() {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;

};

#define MutexLockGuard(x) static_assert(false, "missing  mutex guard var name, 只是创建了临时对象");

}

#endif
