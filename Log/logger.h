#pragma once

#include "Learn-Muduo/Log/logstream.h"
#include "Learn-Muduo/Log/asynclogging.h"

#include <functional>
#include <string.h>

namespace bing {

class Logger {
 public:
    // 日志级别
    enum LogLevel {
        TRACE, DEBUG, INFO, WARN, EROR, FATAL, NUM_LOG_LEVELS,
    };

    // 内部类， 文件相关操作
    class SourceFile {
    public:
        SourceFile(const char* file) : file_(file)
        {
            // 获得file中最后一个 / 的字符
            const char* slash = strrchr(file, '/');
            if (slash) {
                file = slash + 1;       // char*指针指向最终的文件名称
            }
            size_ = static_cast<int>(strlen(file));
        }

        const char* file_;      // 文件名
        int size_;              // 文件的大小
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func_name);

    ~Logger();

    LogStream &stream() { return impl_.stream_; }

    // 当前的日志级别
    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);
    static void setAsync();


    // 输出方法的回调
    using OutputFunc = std::function<void(const LogStream::Buffer &)>;

    static void setOutputFunc(OutputFunc func);

    // 内部类：日志消息的格式
    class Impl {
    public:
        using LogLevel = Logger::LogLevel;
        Impl(LogLevel level, const SourceFile& file, int line);

        // 格式化时间
        void formatTime();

        // 获得线程ID
        void getThreadId();

        int64_t time_;          // 当前时间
        LogStream stream_;      // 输出流（内含缓存一条日志的缓冲区）
        LogLevel level_;        // 日志等级
        SourceFile file_;       // 文件名
        int line_;              // 当前行
    };

 private:
    Impl impl_;                 // Impl 对象
    static bool is_async_;      // 是否使用异步日志

};

// 全局变量：当前的日志级别
extern Logger::LogLevel g_log_level;
extern bool g_is_async_;

// 返回当前的日志级别
inline Logger::LogLevel Logger::logLevel() {
    return g_log_level;
}
// 设置为异步日志
inline void Logger::setAsync() {
    g_is_async_ = true;
}


/* 用户调用的宏 */

// 设置当前的日志级别
#define SET_LOGLEVEL(x) Logger::setLogLevel(x);

// 设置为异步日志
#define LOG_SET_ASYNC(x)                                                \
     if (x != 0)                                                                                \
    {                                                                                          \
        static AsyncLogging g_async_;                                                          \
        Logger::setOutputFunc(                                                                 \
            [&](const LogStream::Buffer &buf) { g_async_.append(buf.data(), buf.length()); }); \
        Logger::setAsync();                                                                    \
    }
#define LOG_TRACE                            \
    if (Logger::logLevel() <= Logger::TRACE) \
    (Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream())

#define LOG_DEBUG                            \
    if (Logger::logLevel() <= Logger::DEBUG) \
    (Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream())

#define LOG_INFO                            \
    if (Logger::logLevel() <= Logger::INFO) \
    (Logger(__FILE__, __LINE__, Logger::INFO, __func__).stream())

#define LOG_WARN logger(__FILE__, __LINE__, Logger::WARN, __func__).stream()
#define LOG_ERROR logger(__FILE__, __LINE__, Logger::ERROR, __func__).stream()
#define LOG_FATAL logger(__FILE__, __LINE__, Logger::FATAL, __func__).stream()




}   // namespace bing 