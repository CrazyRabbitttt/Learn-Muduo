#ifndef BING_BASE_THREAD_H
#define BING_BASE_THREAD_H

#include "nocopyable.h"
#include "CountDownLatch.h"


#include <functional>
// #include <pthread.h>    //use cpp11 thread 
#include <thread>
#include <atomic>
#include <unistd.h>
#include <string>
#include <memory>

namespace bing {

class CountDownLatch;

class Thread : nocopyable{ 
 public: 

   using ThreadFunc = std::function<void()>;
   //  typedef std::function<void ()> ThreadFunc;

    explicit Thread(ThreadFunc func, const std::string& name = std::string()) ;

    ~Thread();

    void start();

    void join();

    bool isRunning() { return running_; }

    pid_t tid() { return tid_ ;}


 private:
    void setDefaultName();
    bool running_;                     //开始线程
    bool joined_;                      //主线程等待子线程运行完毕

    pid_t tid_;                        //线程的标识
   //  pthread_t pid_;                 //进行线程的操作的句柄

   /*
      如果直接调用std::thread thread_, 
      就直接启动线程了，用智能指针进行封装
   */
    std::shared_ptr<std::thread> thread_;       //创建线程，用智能指针包装
    std::string name_;                 //线程的名称
    ThreadFunc func_;                  //线程的执行函数

    CountDownLatch* latch_;

    int nonuse_var;

};  


}   //namespace bing 
#endif