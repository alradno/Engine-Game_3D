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

    // Filtra o clasifica según la severidad:
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        Logger::Error(oss.str());
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        Logger::Warning(oss.str());
    }
    else if (severity == GL_DEBUG_SEVERITY_LOW)
    {
        Logger::Info(oss.str());
    }
    else
    { // GL_DEBUG_SEVERITY_NOTIFICATION
        Logger::Debug(oss.str());
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