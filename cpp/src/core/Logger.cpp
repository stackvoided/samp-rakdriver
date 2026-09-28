#include "../../include/core/Logger.h"

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    switch (level) {
        case LogLevel::Info:    std::cout << "[INFO] " << message << std::endl; break;
        case LogLevel::Warning: std::cout << "[WARN] " << message << std::endl; break;
        case LogLevel::Error:   std::cerr << "[ERR]  " << message << std::endl; break;
        case LogLevel::Debug:   std::cout << "[DBG]  " << message << std::endl; break;
    }
}
