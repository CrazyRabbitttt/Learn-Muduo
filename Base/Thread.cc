#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/CurrentThread.h"

#include <semaphore.h>

using namespace bing::currentThread;

//静态成员变量在类外初始化
namespace bing {


Thread::Thread(ThreadFunc func, const std::string& name) 
    : running_(false), joined_(false), tid_(0), func_(std::move(func)), latch_(1), name_(name)
    {
        //默认latch(1), 等待子线程运行
        setDefaultName();       //设施默认的名字
        printf("Thread class created...\n");
        printf("111.\n");
        func_();
        printf("222.\n");
 
    }

Thread::~Thread() {
    //线程运行了才进行析构
    //joined_是主线程需要等待子线程结束
    //如果不是的话就是守护线程，detach 
    //没有等待子线程结束，直接detach自生自灭就好了
    if (running_ && !joined_) {
        thread_->detach();          //不用等待子线程结束的话， 直接分离就好了
    }

    printf("Thread class destory...\n");
}

//等待子线程退出
void Thread::join() {
    assert(running_);
    assert(!joined_);
    joined_ = true;
    thread_->join();
}

void Thread::start() {
    running_ = true;

    /*
        创建线程， thread_指针指向这个线程的对象，
        使用lambda捕获线程对象，访问线程的成员变量
    */
   
   thread_ = std::shared_ptr<std::thread>(new std::thread([&]() {
        //获得线程的标识符号
        tid_ = currentThread::tid();

        func_();
        printf("The current thread, tid: %d\n", tid_);

        func_();

        // 线程类创建的时候就设定了latch(1), 减1
        // latch_.countDown();
        
   }
    ));
        
    // latch_.wait();          //等待计数器减少为1就可以结束了，要不然子线程函数没有运行
}



void Thread::setDefaultName() { 
    //从线程数目中获得名字
    if (name_.empty()) {
        char buf[32] = {0};
        snprintf(buf, sizeof buf, "Thread");  
        name_ = buf;
    }
}

}  //namespace bing 

