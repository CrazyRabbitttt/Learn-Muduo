#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/CurrentThread.h"

using namespace bing;

//静态成员变量在类外初始化
static std::atomic_int32_t numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string& name) 
    : running_(false), joined_(false), tid_(0), func_(std::move(func)), latch_(1), name_(name)
    {
        setDefaultName();       //设施默认的名字
    }

Thread::~Thread() {
    //线程运行了才进行析构
    //joined_是主线程需要等待子线程结束
    //如果不是的话就是守护线程，detach 
    //没有等待子线程结束，直接detach自生自灭就好了
    if (running_ && !joined_) {
        thread_->detach();          //不用等待子线程结束的话， 直接分离就好了
    }
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

        //线程类创建的时候就设定了latch(1)
        latch_.countDown();
        
        //运行函数
        func_();
   }
    ));

    latch_.wait();

}



void Thread::setDefaultName() {
    //从线程数目中获得名字
    int num = ++numCreated_;
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }

}



