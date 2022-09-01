#include "asynclogging.h"
#include "Logfile.h"

#include <functional>
#include <memory>
#include <unistd.h>

using namespace bing;

AsyncLogging::AsyncLogging(int flush_interval, int rollsize)
    :flush_interval_(flush_interval), 
    roll_size_(rollsize),
    running_(false),            // 开启异步写日志的线程
    thread_(std::bind(&AsyncLogging::writeThread, this), "AsyncLogging Thread"), 
    mutex_(),
    cond_(mutex_),
    current_buffer_(new Buffer),
    next_buffer_(new Buffer),
    buffers_(),
    latch_(1)           // 等线程开始的时候countDown
{
    current_buffer_->bzero();
    next_buffer_->bzero();
    buffers_.reserve(8);                    // cap = 8
}


// 所有的Log都会调用append, 前端写Log
void AsyncLogging::append(const char* buf, int len) {
    MutexLockGuard lock(mutex_);
    // 如果当前的buffer还是有空间的，直接放到本buffer中
    if (current_buffer_->avial()) {
        // 内部会自动判断够不够的
        current_buffer_->append(buf, len);
    } else {                                                // buffer满了，通知日志线程可写
        buffers_.push_back(std::move(current_buffer_));     // 将满了的buffer添加到buffers中
        if (next_buffer_) {
            current_buffer_ = std::move(next_buffer_);      // 移动语意, 减少拷贝的消耗
        } else {
            // 写入的太快了，缓冲区都满了，申请一个新的
            current_buffer_.reset(new Buffer);              // unique_ptr, 抛弃原来的，生成新的
        }
        // 更换完buffer之后将数据写入
        current_buffer_->append(buf, len);
        // 通知日志线程有数据可写（缓冲区满了才通知）
        cond_.notify();
    }

}



void AsyncLogging::writeThread() {
    latch_.countDown();
    // 创建Buffer
    BufferPtr new_buffer1(new Buffer);
    BufferPtr new_buffer2(new Buffer);
    new_buffer1->bzero();
    new_buffer2->bzero();

    // 获得前端的buffer队列？
    BufferVector buffers_to_write;
    buffers_to_write.reserve(8);


    // 后端写日志的线程创建这个LogFile 
    LogFile output(roll_size_, filename_);             // LogFile      

    while (running_) {
        // 临界区
        {
            MutexLockGuard lock(mutex_);
            if (buffers_to_write.empty()) {
                // 如果没人唤醒的话，等待指定的时间
                cond_.waitForSeconds(flush_interval_);      
            }
            buffers_.push_back(std::move(current_buffer_));         // 将前端的拿到里面
            current_buffer_ = std::move(new_buffer1);               // 前端的current替换为没用过的buffer

            buffers_to_write.swap(buffers_);                        // change the buffers

            if (!next_buffer_) {
                // 前端的缓冲区(用buffer2)替换过去
                next_buffer_ = std::move(new_buffer2);
            }
        } // 退出了临界区

        if (buffers_to_write.size() > 16) {
            // char buf[256];
            // snprintf(buf, sizeof buf, "Dropped log messages %zd larger buffers\n", buffers_to_write.size() - 2);
            // fputs(buf, stderr);
            // 如果说日志太多了，丢弃掉多余的日志，只保留两个buffer
            buffers_to_write.erase(buffers_to_write.begin() + 2, buffers_to_write.end());
        }

        // 将列表中的日志写到文件中
        for (const auto& buffer : buffers_to_write){
            output.append(buffer->data(), static_cast<size_t>(buffer->len()));
        }

        if (buffers_to_write.size() > 2) {
            buffers_to_write.resize(2);
        }

        if (!new_buffer1) {
            // 弹出⏏️一个作为newbuffer1
            new_buffer1 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();

            new_buffer1->reset();       // 清理掉
        }

        if (!new_buffer2) {
            new_buffer2 = std::move(buffers_to_write.back());
            buffers_to_write.pop_back();
            new_buffer2->reset();
        }

        buffers_to_write.clear();
        output.flush();                     // 刷盘
    }
    output.flush();
    
}
