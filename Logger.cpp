#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

class Logger {
private:
    static Logger* instance;
    static mutex mutex_;
    ofstream logFile;

    Logger() {
        logFile.open("logFile.log", ios::out | ios::app);
        if (!logFile.is_open()) {
            cerr << "Failed to open log file" << endl;
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    string getCurrentTime() {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
        return ss.str();
    }

public:
    static Logger* getInstance() {
        lock_guard<mutex> lock(mutex_);
        if (instance == nullptr) {
            instance = new Logger();
        }
        return instance;
    }

    template<typename... Args>
    void log(const char* message, Args... args) {
        lock_guard<mutex> lock(mutex_);
        if (logFile.is_open()) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), message, args...);
            logFile << getCurrentTime() << " - Info: " << buffer << endl;
        } else {
            cerr << "Log file is not open" << endl;
        }
    }

    template<typename... Args>
    void warn(const char* message, Args... args) {
        lock_guard<mutex> lock(mutex_);
        if (logFile.is_open()) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), message, args...);
            logFile << getCurrentTime() << " - Warn: " << buffer << endl;
        } else {
            cerr << "Log file is not open" << endl;
        }
    }

    template<typename... Args>
    void error(const char* message, Args... args) {
        lock_guard<mutex> lock(mutex_);
        if (logFile.is_open()) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), message, args...);
            logFile << getCurrentTime() << " - Error: " << buffer << endl;
        } else {
            cerr << "Log file is not open" << endl;
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};


Logger* Logger::instance = nullptr;
mutex Logger::mutex_;