#ifndef BING_LOG_LOGGER_H
#define BING_LOG_LOGGER_H

#include "logstream.h"
#include "TimeStamp.h"
#include <functional>
#include <string.h>

namespace bing {

class Logger {
 public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };

    // 内部类：对于文件的操作
    class SourceFile {
     public:
        SourceFile(const char* file) : file_(file) {
            const char* slash = strrchr(file_, '/');
            if (slash) {
                file_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(file_));
        }
        const char* file_;      // 文件名
        int size_;              // 文件的大小
    };

    // 写一个日志需要的各种类型的参数，生成临时文件进行写操作
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func_name);

    ~Logger();

    // 返回具体操作的LogStream, 然后写的时候就用重载的<<
    LogStream& stream() { return impl_.logstream_; }

    // 输出方法的回调, 传入buffer, 默认输出到stdout 中
    using OutputFunc = std::function<void(const LogStream::Buffer &)>;

    // static void setOutputFunc(OutputFunc func);

    // 内部类：日志消息的格式？？
    class Impl {
     public:
        using LogLevel = Logger::LogLevel;
        Impl(LogLevel level, const SourceFile& file, int line);

        // 获得格式化的时间
        void forattedTime();

        // 获得线程Id
        void getThreadID();

        LogLevel  level_;       // 日志的等级
        LogStream logstream_;   // 输出流，用本输出流进行输出Log
        TimeStamp time_;        // 时间
        SourceFile file_;       // 文件
        int line_;              // 当前行
    };

 private:
    Impl impl_;                 // Impl 对象
    static bool isAsync_;       // 是否是采用异步的日志
};

}   // namespace bing 




// 全局的函数
bing::Logger::LogLevel loglevel();

void setLogLevel(bing::Logger::LogLevel level);
void setOutputFunc(bing::Logger::OutputFunc func);

#define SET_LOGLEVEL(x) setLogLevel(x);

#define LOG_INFO                                \   
    if (loglevel() <= Logger::INFO)             \
    (Logger(__FILE__, __LINE__, bing::Logger::INFO, __func__).stream())

#define LOG_WARN Logger(__FILE__, __LINE__, bing::Logger::WARN, __func__).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, bing::Logger::ERROR, __func__).stream()

#endif