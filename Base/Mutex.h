#ifndef BING_BASE_MUTEX_H
#define BING_BASE_MUTEX_H
#include "Base/nocopyable.h"
#include <assert.h>
#include <pthread.h>

namespace bing{
class MutexLock {
public:


    MutexLock() {
        pthread_mutex_init(&mutex_, NULL);
    }

    void lock() {
        pthread_mutex_lock(&mutex_);
    }

    void unlock() {
        pthread_mutex_unlock(&mutex_);
    }

    //RAII有必要提供保护的数据的获取
    pthread_mutex_t* getPthreadLock() {
        return &mutex_; 
    }

private:

    friend class Condition;
    pthread_mutex_t mutex_;
};



class MutexLockGuard {
 public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) 
    {
        mutex_.lock();          //局部变量初始化之后就进行
    }   

    ~MutexLockGuard() {
        mutex_.unlock();
    }

 private:
    MutexLock& mutex_;
};  


//防止创建了临时对象就锁住了
#define MutexLockGuard(x) static_assert(false, "missing  mutex guard var name, 只是创建了临时对象");
}
#endif