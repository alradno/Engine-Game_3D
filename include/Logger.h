#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>

enum class LogLevel {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // Sets the minimum log level to print messages.
    static void SetLogLevel(LogLevel level) {
        instance().minLevel = level;
    }
    
    // Enables writing to a file in addition to the console.
    // Updated to use truncation mode so that the log file is cleared each time the program starts.
    static void SetLogFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(instance().mutex_);
        // Open the file with truncation mode instead of append mode.
        instance().logFile.open(filename, std::ios::out | std::ios::trunc);
        if (!instance().logFile.is_open()) {
            std::cerr << "[Logger] ERROR: Could not open log file: " << filename << std::endl;
        }
    }
    
    // Logging functions for each level.
    static void Debug(const std::string& msg) {
        instance().log(LogLevel::DEBUG, msg);
    }
    
    static void Info(const std::string& msg) {
        instance().log(LogLevel::INFO, msg);
    }
    
    static void Warning(const std::string& msg) {
        instance().log(LogLevel::WARNING, msg);
    }
    
    static void Error(const std::string& msg) {
        instance().log(LogLevel::ERROR, msg);
    }
    
private:
    LogLevel minLevel = LogLevel::DEBUG;
    std::ofstream logFile;
    std::mutex mutex_;
    
    Logger() { }
    
    // Gets the singleton instance.
    static Logger& instance() {
        static Logger logger;
        return logger;
    }
    
    // Internal function to format and write the message.
    void log(LogLevel level, const std::string& msg) {
        if (level < minLevel)
            return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        std::string levelStr;
        switch (level) {
            case LogLevel::DEBUG: levelStr = "DEBUG"; break;
            case LogLevel::INFO: levelStr = "INFO"; break;
            case LogLevel::WARNING: levelStr = "WARNING"; break;
            case LogLevel::ERROR: levelStr = "ERROR"; break;
        }
        
        std::ostringstream oss;
        oss << "[" << levelStr << "] " << msg << "\n";
        std::string finalMsg = oss.str();
        
        // Write to the console.
        if (level == LogLevel::ERROR)
            std::cerr << finalMsg;
        else
            std::cout << finalMsg;
        
        // Write to the file if it is open.
        if (logFile.is_open()) {
            logFile << finalMsg;
            logFile.flush();
        }
    }
};

#endif // LOGGER_H