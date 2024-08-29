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

enum LogLevel {
    Info,
    Error,
    Warn
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
    string printType(LogLevel t);

public:
    static Logger* getInstance();

    template<typename... Args>
    void log(LogLevel type, string userName, const char* message, Args... args);

    ~Logger();
};
