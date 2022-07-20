#ifndef MUDUO_BASE_THREADPOOL_H_
#define MUDUO_BASE_THREADPOOL_H_

#include "./Condition.h"
#include "Mutex.h"
#include "./Types.h"

#include <deque>
#include <vector>

namespace Muduo{

class ThreadPool : nocopyable {



private:
    MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;

    string name_;
    

};


}





#endif