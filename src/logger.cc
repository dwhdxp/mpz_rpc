#include "logger.h"

// 启动日志线程，不断循环从队列获取日志，并写入磁盘中的日志文件
Logger::Logger() {
    std::thread writeLogTask([&]() {
        while (true) {
            // 获取当前日期
            time_t now = time(nullptr);
            tm* now_tm = localtime(&now);

            // 文件名
            char fileName[128] {0};
            snprintf(fileName, 128, "%d-%d-%d-log.txt", 
                     now_tm->tm_year + 1900, 
                     now_tm->tm_mon + 1, 
                     now_tm->tm_mday);
            FILE *pf = fopen(fileName, "a+");
            if (pf == nullptr) {
                // std::cout << "logger file:" << fileName << " open error!" << std::endl;
                LOG_ERROR("logger file: %s open error", fileName);
                exit(EXIT_FAILURE);
            }

            // 获取队列中的日志信息
            std::string msg = m_lockQue.Pop();
            // 追加时间和日志等级
            char timeBuf[128] {0};
            snprintf(timeBuf, 128, "%d.%d.%d=>[%s]", 
                     now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec,
                     (m_logLevel == INFO ? "INFO" : "ERROR"));
            msg.insert(0, timeBuf);
            msg += '\n';
            
            // 向文件中写日志
            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    writeLogTask.detach();
}

Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLevel level) {
    m_logLevel = level;
}

// 工作线程向队列写日志
void Logger::Log(std::string msg) {
    m_lockQue.Push(msg);
}