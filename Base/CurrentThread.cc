#include "Learn-Muduo/Base/CurrentThread.h"

namespace bing {

namespace currentThread {
    thread_local int cachedTid_ = 0;
    
    void cacheTid() {
        if (cachedTid_  == 0) { //没有获得线程
            cachedTid_ = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }

    } //namespace currentThread
} //namespace bing 