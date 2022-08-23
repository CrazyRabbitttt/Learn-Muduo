#pragma once

#include "Learn-Muduo/Base/nocopyable.h"
#include "Learn-Muduo/Base/Mutex.h"

#include <stdio.h>
#include <string>
#include <limits.h>             // path_max 4096
#include <memory>

namespace bing {


// 写日志数据到本地的文件
class FileWritter : nocopyable {
 public:
    explicit FileWritter(std::string filename)
        : file_(::fopen(filename.c_str(), "ae")),
        written_bytes_(0)
        {
            // 设置文件流的缓冲区, 写的话首先要写满缓冲区再一次性写出去
            ::setbuffer(file_, buffer_, sizeof (buffer_));
        }
    
    ~FileWritter() { ::fclose(file_); }     // 关闭文件，强制flush缓冲区
        
    // 已经写入日志的字节数目
    off_t writtenBytes() const { return written_bytes_; }

    // 将数据写到缓冲区
    void append(const char* line, const size_t len);

    // 刷新缓冲区到文件中去
    void flush() { ::fflush(file_); }

 private:
    FILE* file_;                // 文件指针
    char buffer_[64 * 1024];    // 文件输出缓冲区， 64KB
    off_t written_bytes_;       // 已经写入到日志的字节数

};

// 内置了uniqueptr->filewritter 
class LogFile : nocopyable{

 public:
    LogFile(off_t roll_size);
    ~LogFile();

    void append(const char* line, const size_t len);

    void flush();

    // 滚动日志
    void rollFile();
    

 private:
    void setBaseName();
    std::string getLogFileName();

    char linkname_[PATH_MAX];
    char basename_[PATH_MAX];
    off_t roll_size_;                    // 设置roll 的size
    int file_index_;
    MutexLock mutex_;
    std::unique_ptr<FileWritter> file_;

};








}  // namespace bing 