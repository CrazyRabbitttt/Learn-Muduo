#ifndef LERRNMUDUO_THREAD_H
#define LERRNMUDUO_THREAD_H

#include "./CountDownLatch.h"
#include "./Types.h"
#include "./Nocopyable.h"
#include "Atomic.h"
#include <iostream>
#include <functional>
#include <memory>
#include <pthread.h>

namespace Muduo {

class Thread : nocopyable
{
public:
 typedef std::function<void()> ThreadFunc;

 explicit Thread(ThreadFunc, const string& name = string());

 ~Thread();
  
 void start();
 int join();        //Return pthread_join

 bool started() const { return started_; }

 pid_t tid() const { return tid_; }

 const string& name() const { return name_; }

 static int numCreated() { return numCreated_.get(); }

private:

  void setDefaultName();

  bool started_;              //是否运行
  bool joined_;               //等待线程？
  pthread_t pthreadId_;       //函数使用,线程的返回值，定位线程
  pid_t     tid_;             //线程标识, gettid()  Return thread identification ，任何时刻都是全剧唯一

  ThreadFunc func_;
  string     name_;
  CountDownLatch latch_;      //计时器

  //原子性的
  static AtomicInt32 numCreated_; //线程的序号

};





}

#endif