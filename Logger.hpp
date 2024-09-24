#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

enum class LogLevel {
    INFO,
    ERROR,
    WARN
};

class Logger {
private:
    static Logger* instance;
    static mutex mutex_;
    ofstream logFile;
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    string getCurrentTime();
    string toStringType(LogLevel t);
    ~Logger();

public:
    static Logger* getInstance();

    template<typename... Args>
    void log(LogLevel type, string userName, const char* message, Args... args) {
        lock_guard<mutex> lock(mutex_);
        if (logFile.is_open()) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), message, args...);
            logFile << "By : " << userName << " || " << getCurrentTime() << " - "
                    << toStringType(type) << ": " << buffer << endl;
        } else {
            cerr << "Log file is not open" << endl;
        }
    }
};
