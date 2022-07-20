#include "./Thread.h"


#include <type_traits>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>


namespace Muduo {

namespace Detail {

    pid_t gettid() {
        return static_cast<pid_t>(::syscall(SYS_gettid));           //调用linux的syscall, 
    }

    //用于进行初始化线程的工具类？
    struct ThreadData {
        typedef Muduo::Thread::ThreadFunc ThreadFunc;
        ThreadFunc func_;
        string name_;
        pid_t* tid_;
        CountDownLatch* latch_;      

        ThreadData(ThreadFunc func, const string& name, pid_t* tid,
        CountDownLatch*latch)
            :func_(std::move(func)), name_(name), tid_(tid), latch_(latch) {}

        void runInThread() {






            
        }

    };

    //也就是线程的运行函数，返回的是void*类型
    void* startThread(void* obj) {
        ThreadData* data = static_cast<ThreadData*>(obj);
        data->runInThread();    
        delete data;
        return NULL;
    }


}  //namespace detail 



    Thread::Thread(ThreadFunc func, const string& n) 
        :started_(false), joined_(false), pthreadId_(0),
        tid_(0), func_(std::move(func)),
        name_(n), latch_(1)
        {
            //创建的时候设置名称
            setDefaultName();
        }

    Thread::~Thread() {
        if (started_ && !joined_) {
            pthread_detach(pthreadId_);         //自动进行分离
        }
    }


    //线程开始的话，就是需要将一些内部的数据传过去，因此再次抽象了一个保存数据的struct 
    void Thread::start() {
        assert(!started_);
        started_ = true;

        Detail::ThreadData* data = new Detail::ThreadData(func_, name_, &tid_, &latch_);
        if (pthread_create(&pthreadId_, NULL, &Detail::startThread, data)) {

        }

    }



    //线程根据线程的序号进行设置线程名字
    void Thread::setDefaultName() {
        int num = numCreated_.incrementAndGet();
        if (name_.empty()) {
            char buf[32];
            snprintf(buf, sizeof buf, "Thread%d", num);
            name_ = buf;
        }
    }



}