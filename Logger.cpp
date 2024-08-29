#include "Logger.hpp"

Logger* Logger::instance = nullptr;
mutex Logger::mutex_;

Logger::Logger() {
    logFile.open("logFile.log", ios::out | ios::app);
    if (!logFile.is_open()) {
        cerr << "Failed to open log file" << endl;
    }
}

string Logger::getCurrentTime() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

string Logger::printType(LogLevel t) {
    switch (t) {
        case Info:
            return "Info";
        case Warn:
            return "Warn";
        case Error:
            return "Error";
        default:
            return "Unknown";
    }
}

Logger* Logger::getInstance() {
    lock_guard<mutex> lock(mutex_);
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

template<typename... Args>
void Logger::log(LogLevel type, string userName, const char* message, Args... args) {
    lock_guard<mutex> lock(mutex_);
    if (logFile.is_open()) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), message, args...);
        logFile << "By : " << userName << " || " << getCurrentTime() << " - "
                << printType(type) << ": " << buffer << endl;
    } else {
        cerr << "Log file is not open" << endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}
