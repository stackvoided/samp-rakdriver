#pragma once

#include <iostream>
#include <mutex>
#include <string>

enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug
};

class Logger {
public:
    static Logger& Instance();
    void Log(LogLevel level, const std::string& message);

private:
    Logger() = default;
    std::mutex m_mutex;
};

#define LOG_INFO(msg) Logger::Instance().Log(LogLevel::Info, msg)
#define LOG_WARN(msg) Logger::Instance().Log(LogLevel::Warning, msg)
#define LOG_ERR(msg) Logger::Instance().Log(LogLevel::Error, msg)
#define LOG_DEBUG(msg) Logger::Instance().Log(LogLevel::Debug, msg)
