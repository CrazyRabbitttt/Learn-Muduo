#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/CurrentThread.h"

using namespace bing;

static std::atomic_int32_t numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string& name) 
    : running_(false), joined_(false), pid_(0), tid_(0), func_(std::move(func)), latch_(1), name_(name)
    {
        setDefaultName();       //设施默认的名字
    }

Thread::~Thread() {
    //线程运行了才进行西沟
    //joined_是主线程需要等待子线程结束
    //如果不是的话就是守护线程，detach 
    //没有等待子线程结束，直接detach自生自灭就好了
    if (running_ && !joined_) {
        pthread_detach(pid_);       //直接分离子线程就行了
    }
}

//等待子线程退出
void Thread::join() {
    joined_ = true;
    pthread_join(pid_, NULL);
}


void Thread::start() {
    assert(!running_);
    running_ = true;

    if (pthread_create(&pid_, NULL, &func_, NULL)) {
        //创建失败了 
    } else {
        latch_.wait();      //等待计数为0
        assert(tid_ > 0);
    }


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



