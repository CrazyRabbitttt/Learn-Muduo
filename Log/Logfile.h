#ifndef BING_LOG_LOGFILE_H
#define BING_LOG_LOGFILE_H

#include "nocopyable.h"
#include "Mutex.h"
#include "TimeStamp.h"

#include <stdio.h>
#include <memory>
#include <string>


namespace bing {

class FileWritter : nocopyable{

 public:
    explicit FileWritter(std::string filename)
        : writtenbytes_(0)//, file_(::fopen(filename.c_str(), "ae"))
    {   
        if (filename != "") {
            std::string NAme("./LogFiles/");
            NAme += filename;
            NAme += ".log";
            file_ = ::fopen(NAme.data(), "ae");
        } else {
            std::string defaultPathName = "./LogFiles/LogFile_" + TimeStamp::now().toString() + ".log";
            file_ = ::fopen(defaultPathName.data(), "ae");
        }

        // 设置文件读写的缓冲区，
        ::setbuffer(file_, buffer_, sizeof buffer_);
    }   

    ~FileWritter() {
        ::fclose(file_);
    }   

    // 写数据到缓冲区中
    void append(const char* dataline, const size_t len);

    // 将缓冲区的内容刷盘
    void fflush() { ::fflush(file_); }              

    off_t writtentBytes() const { return writtenbytes_; }

 private:
    FILE *file_;                // 文件的读写指针
    char buffer_[64 * 1000];    // buffer, 64KB
    off_t writtenbytes_;          // 已经写了的数据的大小

};


class LogFile : nocopyable{
 public:
    LogFile(off_t roll_size, std::string& filename);
    ~LogFile();

    void append(const char *line, const size_t len);
    void flush();

    // 滚动日志，将很久的日志覆盖掉？
    void rollFile();

 private:
    MutexLock mutex_;
    std::unique_ptr<FileWritter> file_;
    off_t roll_size_;
    std::string filename_;
};




}   // namesapce bing 

#endif