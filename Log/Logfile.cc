#include "Learn-Muduo/Log/Logfile.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

using namespace bing;

// 写数据到缓冲区中
void FileWritter::append(const char* line, const size_t len) {
    size_t n = fwrite(line, 1, len, file_);     // 偏离line的位置
    size_t remain = n;      // 剩余没写的
    while (remain > 0) {
        size_t x = fwrite(line + n, 1, remain, file_);
        if (x == 0) {
            int err = ferror(file_);
            if (err) {
                fprintf(stderr, "FIleWritter::append() failed %d\n", err);
            }
            break;
        }
        n += x;     
        remain -= x;
    }
    // 到这就是全写完了
    written_bytes_ += len;
}

LogFile::LogFile(off_t roll_size) 
    :roll_size_(roll_size),     // 日志文件的滚动大小
     file_index_(0)
{
    setBaseName();
    rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char* line, const size_t len) {
    MutexLockGuard lock(mutex_);
    file_->append(line, len);       // 调用底层的append指针
    if (file_->writtenBytes() > roll_size_) {
        rollFile();
    }
}

void LogFile::flush() {
    MutexLockGuard lock(mutex_);
    file_->flush();
}

// 滚动日志，重新生成日志，向里面写数据
void LogFile::rollFile() {
    std::string file_name = getLogFileName();

    // 指向新的文件
    file_.reset(new FileWritter(file_name.c_str()));
    unlink(linkname_);
    symlink(file_name.c_str(), linkname_);
}

void LogFile::setBaseName() {
    char log_abs_path[PATH_MAX] = {0};
    ::getcwd(log_abs_path, sizeof(log_abs_path));
    strcat(log_abs_path, "/log/");
    if (::access(log_abs_path, 0) == -1)
    {
        ::mkdir(log_abs_path, 0755);
    }

    char process_abs_path[PATH_MAX] = {0};
    long len = ::readlink("/proc/sel/exe", process_abs_path, sizeof(process_abs_path));
    if (len <= 0)
    {
        return;
    }
    char *process_name = strrchr(process_abs_path, '/') + 1;
    snprintf(linkname_, sizeof(linkname_), "%s%s.log", log_abs_path, process_name);
    snprintf(basename_, sizeof(basename_), "%s%s.%d", log_abs_path, process_name, ::getpid());
}


std::string LogFile::getLogFileName() {
    std::string file_name(basename_);
    char timebuf[32] = {0};
    struct tm tm;
    time_t now = time(nullptr);
    ::gmtime_r(&now, &tm);
    strftime(timebuf, sizeof(timebuf), "%Y%m%d-%H%M%S.", &tm);
    file_name += timebuf;

    char index[8] = {0};
    snprintf(index, sizeof(index), "%3d.log", file_index_);
    ++file_index_;
    file_name += index;
    return file_name;
}

