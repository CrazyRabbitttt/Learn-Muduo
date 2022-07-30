#ifndef BING_BASE_THREAD_H
#define BING_BASE_THREAD_H

#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/CountDownLatch.h"


#include <functional>
#include <pthread.h>
#include <atomic>
#include <unistd.h>
#include <string>
#include <memory>

namespace bing {
class Thread : nocopyable{ 
 public: 
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const std::string& name = std::string());

    ~Thread();

    void start();

    void join();

    bool isRunning() { return running_; }

    pid_t tid() { return tid_ ;}

 private:
    void setDefaultName();
    
    pid_t tid_;              //线程的标识
    pthread_t pid_;           //进行线程的操作的句柄
    std::string name_;       //线程的名称
    ThreadFunc func_;        //线程的执行函数
    bool running_;         
    bool joined_;             //主线程等待子线程
    CountDownLatch latch_;

    static std::atomic_int32_t numCreated_; // 记录产生线程的个数(静态变量)
};  


}   //namespace bing 
#endif