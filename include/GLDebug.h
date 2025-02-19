#pragma once
#include <glad/glad.h>
#include "Logger.h"
#include <sstream>

// Función para limpiar todos los errores previos de OpenGL.
inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

// Función para loguear errores en las llamadas a OpenGL.
inline bool GLLogCall(const char *function, const char *file, int line)
{
    bool success = true;
    while (GLenum error = glGetError())
    {
        std::ostringstream oss;
        oss << "[OpenGL Error] (" << error << "): " << function << " in " << file << ":" << line;
        Logger::Error(oss.str());
        success = false;
    }
    return success;
}

#ifdef _DEBUG
// Macro usando una lambda de invocación inmediata para encapsular todas las instrucciones.
#define GLCall(x) ([&]() { \
        GLClearError(); \
        x; \
        if (!GLLogCall(#x, __FILE__, __LINE__)) { __debugbreak(); } }())
#else
#define GLCall(x) x
#endif

// Callback para mensajes de depuración de OpenGL (funciona si el driver lo soporta).
inline void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar *message, const void *userParam)
{
    std::ostringstream oss;
    oss << "[OpenGL Debug] " << message;
    std::string logMsg = oss.str();
    // Usar el id del mensaje en la key para distinguir distintos avisos
    std::string key = "OpenGL_Debug_" + std::to_string(id);

    // Se asignan intervalos según la severidad:
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        // Mensajes críticos: se muestran como error, pero solo cada 5 segundos.
        Logger::ThrottledLog(key, LogLevel::ERROR, logMsg, 5.0);
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        // Advertencias de nivel medio: se muestran como warning, cada 2 segundos.
        Logger::ThrottledLog(key, LogLevel::WARNING, logMsg, 2.0);
    }
    else if (severity == GL_DEBUG_SEVERITY_LOW)
    {
        // Advertencias de nivel bajo: se muestran como info, cada 0.5 segundos.
        Logger::ThrottledLog(key, LogLevel::INFO, logMsg, 0.5);
    }
    else
    { // GL_DEBUG_SEVERITY_NOTIFICATION
        // Notificaciones: se muestran como debug, cada 0.5 segundos.
        Logger::ThrottledLog(key, LogLevel::DEBUG, logMsg, 0.5);
    }
}

// Función para configurar el callback de depuración de OpenGL.
inline void SetupOpenGLDebugCallback()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}