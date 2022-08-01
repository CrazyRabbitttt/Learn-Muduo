#ifndef BING_BASE_CURRENTTHREAD_H
#define BING_BASE_CURRENTTHREAD_H

#include <unistd.h>
#include <sys/syscall.h>


//获得当前的线程id
namespace bing {
  namespace currentThread {
  //thread local 变量, 每个线程都有的全局变量
  extern thread_local int cachedTid_;     //extern, 能够被别的文件引用的线程局部全局变量


  //获得线程id
  void cacheTid();

  inline int tid() {
        //没有获得线程的id
      if (__builtin_expect(cachedTid_ == 0, 0)) {
        cacheTid();
      }
      return cachedTid_;
  }

  } //namespace currentThread

}  //namespace bing 

#endif