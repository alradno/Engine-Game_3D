/**
 * @file Logger.h
 * @brief Logger singleton that writes logs to a main file (all messages) as well as separate files for INFO, DEBUG, and ERROR messages.
 */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>

enum class LogLevel
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:
    // Set the minimum log level to output messages.
    static void SetLogLevel(LogLevel level)
    {
        instance().minLevel = level;
    }

    /**
     * @brief Sets the main log file and automatically creates three additional log files for INFO, DEBUG, and ERROR messages.
     * @param filename The name of the main log file.
     */
    static void SetLogFile(const std::string &filename)
    {
        std::lock_guard<std::mutex> lock(instance().mutex_);
        // Open the main log file
        instance().logFile.open(filename, std::ios::out | std::ios::trunc);
        if (!instance().logFile.is_open())
        {
            std::cerr << "[Logger] ERROR: Could not open log file: " << filename << std::endl;
        }

        // Helper lambda to generate file name with suffix
        auto getSuffixLogFileName = [&](const std::string &base, const std::string &suffix) -> std::string
        {
            size_t pos = base.find_last_of('.');
            if (pos == std::string::npos)
                return base + "_" + suffix + ".log";
            else
            {
                std::string name = base.substr(0, pos);
                std::string ext = base.substr(pos);
                return name + "_" + suffix + ext;
            }
        };

        // Open additional log files
        instance().infoFile.open(getSuffixLogFileName(filename, "info"), std::ios::out | std::ios::trunc);
        if (!instance().infoFile.is_open())
            std::cerr << "[Logger] ERROR: Could not open info log file." << std::endl;

        instance().debugFile.open(getSuffixLogFileName(filename, "debug"), std::ios::out | std::ios::trunc);
        if (!instance().debugFile.is_open())
            std::cerr << "[Logger] ERROR: Could not open debug log file." << std::endl;

        instance().errorFile.open(getSuffixLogFileName(filename, "error"), std::ios::out | std::ios::trunc);
        if (!instance().errorFile.is_open())
            std::cerr << "[Logger] ERROR: Could not open error log file." << std::endl;
    }

    static void Debug(const std::string &msg)
    {
        instance().log(LogLevel::DEBUG, msg);
    }

    static void Info(const std::string &msg)
    {
        instance().log(LogLevel::INFO, msg);
    }

    static void Warning(const std::string &msg)
    {
        instance().log(LogLevel::WARNING, msg);
    }

    static void Error(const std::string &msg)
    {
        instance().log(LogLevel::ERROR, msg);
    }

private:
    LogLevel minLevel = LogLevel::DEBUG;
    std::ofstream logFile;
    std::ofstream infoFile;
    std::ofstream debugFile;
    std::ofstream errorFile;
    std::mutex mutex_;

    Logger() {}

    static Logger &instance()
    {
        static Logger logger;
        return logger;
    }

    void log(LogLevel level, const std::string &msg)
    {
        if (level < minLevel)
            return;

        std::lock_guard<std::mutex> lock(mutex_);
        std::string levelStr;
        switch (level)
        {
        case LogLevel::DEBUG:
            levelStr = "DEBUG";
            break;
        case LogLevel::INFO:
            levelStr = "INFO";
            break;
        case LogLevel::WARNING:
            levelStr = "WARNING";
            break;
        case LogLevel::ERROR:
            levelStr = "ERROR";
            break;
        }

        std::ostringstream oss;
        oss << "[" << levelStr << "] " << msg << "\n";
        std::string finalMsg = oss.str();

        // Write to console
        if (level == LogLevel::ERROR)
            std::cerr << finalMsg;
        else
            std::cout << finalMsg;

        // Write to main log file
        if (logFile.is_open())
        {
            logFile << finalMsg;
            logFile.flush();
        }

        // Write to specific log files based on level
        if (level == LogLevel::INFO && infoFile.is_open())
        {
            infoFile << finalMsg;
            infoFile.flush();
        }
        if (level == LogLevel::DEBUG && debugFile.is_open())
        {
            debugFile << finalMsg;
            debugFile.flush();
        }
        if (level == LogLevel::ERROR && errorFile.is_open())
        {
            errorFile << finalMsg;
            errorFile.flush();
        }
    }
};
