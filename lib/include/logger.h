#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include <thread>
#include "lockqueue.h"

class Logger;

#define LOG_INFO(format, ...)                           \
    do {                                                \
        Logger &logger = Logger::GetInstance();         \
        logger.setLogLevel(INFO);                       \
        char buffer[1024] {0};                          \
        snprintf(buffer, 1024, format, ##__VA_ARGS__);  \
        logger.Log(buffer);                             \
    } while (0);
    
#define LOG_ERROR(format, ...)                          \
    do {                                                \
        Logger &logger = Logger::GetInstance();         \
        logger.setLogLevel(ERROR);                      \
        char buffer[1024] {0};                          \
        snprintf(buffer, 1024, format, ##__VA_ARGS__);  \
        logger.Log(buffer);                             \
    } while (0);

enum LogLevel {
    INFO,
    ERROR,
};

// 日志类：单例模式
class Logger {
public:
    static Logger& GetInstance();
    void setLogLevel(LogLevel level);
    void Log(std::string msg);

private:
    int m_logLevel;
    LockQueue<std::string> m_lockQue;

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};