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
    // Establece el nivel mínimo de mensajes a imprimir.
    static void SetLogLevel(LogLevel level) {
        instance().minLevel = level;
    }
    
    // Habilita la escritura en un archivo además de la consola.
    static void SetLogFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(instance().mutex_);
        instance().logFile.open(filename, std::ios::out | std::ios::app);
        if (!instance().logFile.is_open()) {
            std::cerr << "[Logger] ERROR: Could not open log file: " << filename << std::endl;
        }
    }
    
    // Funciones de logging para cada nivel.
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
    
    // Obtiene la instancia singleton.
    static Logger& instance() {
        static Logger logger;
        return logger;
    }
    
    // Función interna para formatear y escribir el mensaje.
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
        
        // Escribir a la consola
        if (level == LogLevel::ERROR)
            std::cerr << finalMsg;
        else
            std::cout << finalMsg;
        
        // Escribir al archivo si está abierto.
        if (logFile.is_open()) {
            logFile << finalMsg;
            logFile.flush();
        }
    }
};

#endif // LOGGER_H