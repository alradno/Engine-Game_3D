#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include <glad/glad.h>
#include <iostream>
#include "Logger.h"

// UniformBuffer encapsulates an OpenGL Uniform Buffer Object (UBO).
// It provides methods for generating, binding, setting data, and binding to a specified binding point.
class UniformBuffer {
public:
    unsigned int ID; // The OpenGL ID for the uniform buffer.

    // Constructor: Generates the UBO and logs its creation.
    UniformBuffer() {
        glGenBuffers(1, &ID);
        Logger::Debug("[UniformBuffer] Generated UBO ID: " + std::to_string(ID));
    }

    // Binds the UBO to the GL_UNIFORM_BUFFER target.
    void Bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
    }

    // Unbinds any buffer from the GL_UNIFORM_BUFFER target.
    void Unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // Sets the data for the UBO.
    // @param size: The size of the data in bytes.
    // @param data: Pointer to the data to be stored.
    // @param usage: Usage pattern of the data store (e.g., GL_STATIC_DRAW).
    void SetData(GLsizeiptr size, const void* data, GLenum usage) {
        Bind();
        glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
        Logger::Info("[UniformBuffer] Data set (size: " + std::to_string(size) + " bytes).");
        Unbind();
    }

    // Binds the UBO to a specific binding point.
    // @param bindingPoint: The binding point to which the UBO is bound.
    void BindToPoint(GLuint bindingPoint) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ID);
        Logger::Debug("[UniformBuffer] UBO bound to binding point " + std::to_string(bindingPoint));
    }
};

#endif // UNIFORMBUFFER_H