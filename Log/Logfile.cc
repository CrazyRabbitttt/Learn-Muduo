#include "Learn-Muduo/Log/Logfile.h"

using namespace bing;

void FileWritter::append(const char* line, const size_t len) {
    size_t n = ::fwrite(line, 1, len, file_);
    int remain = len - n;
    // n算是line中偏移的位置， 也即是下一个待写入的位置
    while (remain > 0) {
        size_t num = ::fwrite(line + n, 1, remain, file_);      // 继续写
        if (num == 0) {
            int err = ferror(file_);
            if (err) {
                fprintf(stderr, "FileWritter::append() failed %d\n", err);
            }
            break;
        }
        remain -= num;
        n += num;
    }
    writtenbytes_ += len;
}


LogFile::LogFile(off_t roll_size, std::string& filename) 
    : roll_size_(roll_size), file_(new FileWritter(filename))
{}


LogFile::~LogFile() {}


void LogFile::append(const char* line, const size_t len) {
    MutexLockGuard lock(mutex_);
    file_->append(line, len);
}

void LogFile::flush() {
    MutexLockGuard lock(mutex_);
    file_->fflush();
}





