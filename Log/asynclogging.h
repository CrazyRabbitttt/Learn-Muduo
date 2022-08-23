#pragma once

// 前端写日志的操作API， 有两个缓冲区

#include "Learn-Muduo/Log/logstream.h"
#include "Learn-Muduo/Base/Mutex.h"
#include "Learn-Muduo/Base/Thread.h"
#include "Learn-Muduo/Base/CountDownLatch.h"
#include "Learn-Muduo/Base/nocopyable.h"

#include <vector>
#include <memory>
#include <atomic>
#include <thread>

namespace bing {

class AsyncLogging : nocopyable {
    using Buffer = LogBuffer<kLargeBuffer>;     // 较大的buffer
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;
    using BufferPtr = BufferVector::value_type;

 public:
    AsyncLogging(int flush_interval = 500, int rollsize = 20 * 1024 * 1024);
    ~AsyncLogging() {
        if (running_) {
            stop();
        }
    }

    void append(const char* buf, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();                      // 等待写的线程开启
    }

    void stop() {
        running_ = false;
        cond_.notify();                     // 通知一下，结束了
        thread_.join();                     // 等待线程结束
    }

 private:
    void writeThread();

    const int flush_interval_;                  // 定时缓冲的时间
    const int roll_size_;                       // 缓冲额定空间
    std::atomic<bool> running_;                 // 

    Thread thread_;                         
    MutexLock mutex_;
    Condition cond_;
    CountDownLatch latch_;
    // std::thread thread_;                        // 异步写日志的线程
    // std::mutex mutex_;                          
    // std::condition_variable cond_;  



    BufferPtr current_buffer_;                  // 当前缓冲区
    BufferPtr next_buffer_;                     // 预备缓冲区
    BufferVector buffers_;                      // buffer队列，都是待写入后端的， 待写入文件

};

}       // namespace bing 
