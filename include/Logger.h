/**
 * @file Logger.h
 * @brief Logger singleton que escribe logs a un archivo principal y archivos separados para INFO, DEBUG, WARNING y ERROR.
 *
 * Se ha extendido para incluir funciones de throttling y threshold que permiten emitir mensajes
 * solo cuando han pasado cierto tiempo o cuando los valores cambian de forma significativa.
 * Además, se incluye el parámetro "limitLog" (yes/no) para habilitar o deshabilitar la limitación de logs.
 */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex> // Usamos std::recursive_mutex
#include <string>
#include <unordered_map>
#include <chrono>
#include <cmath>

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
    // Permite habilitar o deshabilitar la limitación de logs (throttling y threshold).
    // Si limitLog es true, se aplican las limitaciones; si es false, se loguea todo sin throttling.
    static void SetLimitLog(bool limit)
    {
        instance().limitLog = limit;
    }

    // Funciones para configurar el nivel y los archivos de log
    static void SetLogLevel(LogLevel level)
    {
        instance().minLevel = level;
    }

    static void SetLogFile(const std::string &filename)
    {
        std::lock_guard<std::recursive_mutex> lock(instance().mutex_);
        // Abrir el archivo principal
        instance().logFile.open(filename, std::ios::out | std::ios::trunc);
        if (!instance().logFile.is_open())
        {
            std::cerr << "[Logger] ERROR: Could not open log file: " << filename << std::endl;
        }

        // Lambda para obtener el nombre del archivo con sufijo
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

        instance().infoFile.open(getSuffixLogFileName(filename, "info"), std::ios::out | std::ios::trunc);
        if (!instance().infoFile.is_open())
            std::cerr << "[Logger] ERROR: Could not open info log file." << std::endl;

        instance().debugFile.open(getSuffixLogFileName(filename, "debug"), std::ios::out | std::ios::trunc);
        if (!instance().debugFile.is_open())
            std::cerr << "[Logger] ERROR: Could not open debug log file." << std::endl;

        instance().warningFile.open(getSuffixLogFileName(filename, "warning"), std::ios::out | std::ios::trunc);
        if (!instance().warningFile.is_open())
            std::cerr << "[Logger] ERROR: Could not open warning log file." << std::endl;

        instance().errorFile.open(getSuffixLogFileName(filename, "error"), std::ios::out | std::ios::trunc);
        if (!instance().errorFile.is_open())
            std::cerr << "[Logger] ERROR: Could not open error log file." << std::endl;
    }

    // Funciones básicas de log
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

    // NUEVAS FUNCIONES DE LOGGING CON THROTTLING Y THRESHOLD

    /**
     * @brief Emite un log de forma "throttled" (limitado en frecuencia) identificado por 'key'.
     * Solo se escribe el mensaje si ha transcurrido al menos throttleIntervalSeconds desde el último log con esa key.
     */
    static void ThrottledLog(const std::string &key, LogLevel level, const std::string &msg, double throttleIntervalSeconds = 0.5)
    {
        // Si la limitación está desactivada, logueamos inmediatamente.
        if (!instance().limitLog)
        {
            instance().log(level, msg);
            return;
        }
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::recursive_mutex> lock(instance().mutex_);
        auto it = instance().throttledLogTimes.find(key);
        if (it != instance().throttledLogTimes.end())
        {
            double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count() / 1000.0;
            if (elapsed < throttleIntervalSeconds)
            {
                return; // No se emite el log si no se cumple el intervalo
            }
        }
        // Se actualiza el tiempo del último log y se emite el mensaje
        instance().throttledLogTimes[key] = now;
        instance().log(level, msg);
    }

    /**
     * @brief Emite un log si el valor actual cambia más que 'threshold' respecto al último valor registrado con 'key'.
     * Además, se aplica throttling para limitar la frecuencia.
     *
     * @tparam T Tipo numérico (int, float, etc.)
     * @param key Identificador único para el valor a comparar.
     * @param currentValue Valor actual.
     * @param threshold Diferencia mínima para considerar que el cambio es significativo.
     * @param level Nivel del log.
     * @param msg Mensaje a registrar.
     * @param throttleIntervalSeconds Intervalo mínimo de tiempo entre logs para la misma key.
     */
    template <typename T>
    static void ThresholdLog(const std::string &key, T currentValue, T threshold, LogLevel level, const std::string &msg, double throttleIntervalSeconds = 0.5)
    {
        // Si la limitación está desactivada, logueamos inmediatamente.
        if (!instance().limitLog)
        {
            instance().log(level, msg);
            return;
        }
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::recursive_mutex> lock(instance().mutex_);
        bool shouldLog = false;
        auto it = instance().thresholdLogValues.find(key);
        if (it != instance().thresholdLogValues.end())
        {
            if (std::abs(currentValue - it->second) > threshold)
            {
                shouldLog = true;
            }
        }
        else
        {
            shouldLog = true;
        }
        // Se comprueba también el throttling
        auto itTime = instance().throttledLogTimes.find(key);
        if (itTime != instance().throttledLogTimes.end())
        {
            double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - itTime->second).count() / 1000.0;
            if (elapsed < throttleIntervalSeconds)
            {
                shouldLog = false;
            }
        }
        if (shouldLog)
        {
            instance().thresholdLogValues[key] = static_cast<double>(currentValue);
            instance().throttledLogTimes[key] = now;
            instance().log(level, msg);
        }
    }

private:
    LogLevel minLevel = LogLevel::DEBUG;
    std::ofstream logFile;
    std::ofstream infoFile;
    std::ofstream debugFile;
    std::ofstream warningFile;
    std::ofstream errorFile;
    std::recursive_mutex mutex_;

    // Mapas para guardar la última vez que se emitió un log (para throttling)
    // y el último valor registrado (para threshold)
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> throttledLogTimes;
    std::unordered_map<std::string, double> thresholdLogValues;

    // Si limitLog es true, se aplica la limitación de logs (throttling/threshold); de lo contrario, se loguea sin limitar.
    bool limitLog = true;

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

        std::lock_guard<std::recursive_mutex> lock(mutex_);
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

        // Escribir en consola
        if (level == LogLevel::ERROR)
            std::cerr << finalMsg;
        else
            std::cout << finalMsg;

        // Escribir en archivo principal
        if (logFile.is_open())
        {
            logFile << finalMsg;
            logFile.flush();
        }

        // Escribir en archivos específicos según nivel
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
        if (level == LogLevel::WARNING && warningFile.is_open())
        {
            warningFile << finalMsg;
            warningFile.flush();
        }
        if (level == LogLevel::ERROR && errorFile.is_open())
        {
            errorFile << finalMsg;
            errorFile.flush();
        }
    }
};
