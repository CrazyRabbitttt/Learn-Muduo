#include "Learn-Muduo/Log/logger.h"
#include "Learn-Muduo/Base/TimeStamp.h"

using namespace bing;

// 日志级别的名字
const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
    "TRACE ",
    "DEBUG ",
    "INFO ",
    "WARN ",
    "ERROR ",
    "FATAL ",
};


// 设置当前的日志级别

void Logger::setLogLevel(LogLevel level) {
    g_log_level = level;
}

// 默认当前的日志级别是 INFO
Logger::LogLevel g_log_level = Logger::INFO;



