#include "Learn-Muduo/Log/logger.h"
#include "Learn-Muduo/Base/TimeStamp.h"

#include <sys/syscall.h>
#include <unistd.h>

using namespace bing;


// 日志级别对应的名字，通过enum的值对应->string 
const char* LogLevelName[5] = {
        "DEBUG ",
        "INFO  ",
        "WARN  ",
        "ERROR ",
        "FATAL ",
};

void defaultOutPut(const LogStream::Buffer& buffer) {
    // 将buffer 写到标准输出中去
    size_t n = fwrite(buffer.data(), 1, static_cast<int>(buffer.len()), stdout);       // 
    // FIXME:check n
    (void) n;
}

void defaultFlush() {
    fflush(stdout);
}


// 全局的方法： 输出方法的回调，初始化是默认的标准输出
Logger::OutputFunc g_output_func = defaultOutPut;
Logger::LogLevel g_log_level = Logger::INFO;
bool g_is_async = false;

void setOutputFunc(bing::Logger::OutputFunc func) {
    g_output_func = func;
}

void setLogLevel(bing::Logger::LogLevel level) {
    g_log_level = level;
}

Logger::LogLevel loglevel() { return g_log_level; }

class Tmp{
public:
    Tmp(const char* str, unsigned len)
        :str_(str), len_(len)
    {}

    const char* str_;
    const int len_;
};

// 重载输入tmp类型到buf中的函数
inline LogStream& operator<<(LogStream& s, Tmp v) {
    s.append(v.str_, v.len_);
    return s;
}

// 重载输出file运算，
inline LogStream& operator<<(LogStream& s, Logger::SourceFile v) {
    s.append(v.file_, v.size_);
    return s;
}


Logger::Logger(SourceFile file, int line) 
    : impl_(INFO, file, line) {}

Logger::Logger(SourceFile file, int line, LogLevel level)
    : impl_(level, file, line) {}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func_name)
    : impl_(level, file, line) 
{
    impl_.logstream_ << "(" << func_name << "):";   // 将funcname 存放到buffer中
}    


// Logger 对象析构的时候需要将缓冲区的内容进行输出掉, << 只是将内容写到buff, 析构的时候输出内容
Logger::~Logger() {
    // 换行符号写到缓冲区中， 能够强制进行fflush ??
    stream() << "\n";
    const LogStream::Buffer &buf(stream().buffer());
    g_output_func(buf);
}
    
Logger::Impl::Impl(LogLevel level, const SourceFile& file, int line)
    : file_(std::move(file)),
      line_(line),
      level_(level),
      logstream_(),
      time_(std::move(TimeStamp::now()))
{
    forattedTime();                                     // 格式化的时间
    getThreadID();                                      // 线程id
    logstream_ << Tmp(LogLevelName[level_], 6);         // level
    logstream_ << file_ << ':' << line_ << "->";        // File, Line
}
      


void Logger::Impl::forattedTime() {
    time_t seconds = static_cast<time_t>(time_.microSecond() / TimeStamp::kMicroSecond2Second);
    time_t microSecond = static_cast<time_t>(time_.microSecond() % TimeStamp::kMicroSecond2Second);

    // 获得格式化的时间
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);
    char t_time[64] = {0};
    snprintf(t_time, sizeof(t_time), "%04d-%02d-%02d %02d:%02d:%02d", tm_time.tm_year + 1900,
        tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour , tm_time.tm_min,
        tm_time.tm_sec);
    
    // 将毫秒拼接上
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), ".%03d", microSecond);

    // 输出到buffer中
    logstream_ << Tmp(t_time, 19) << Tmp(buf, 4);
}

// 获得当前线程的ID
void Logger::Impl::getThreadID() {
    char buf[32] = {0};
    int len = snprintf(buf, sizeof (buf), "[%5lu]", ::syscall(SYS_gettid));
    logstream_ << Tmp(buf, len);
}
