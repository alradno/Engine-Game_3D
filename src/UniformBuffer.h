#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include <glad/glad.h>
#include <iostream>
#include "Logger.h"

class UniformBuffer {
public:
    unsigned int ID;
    UniformBuffer() {
        glGenBuffers(1, &ID);
        Logger::Debug("[UniformBuffer] Generated UBO ID: " + std::to_string(ID));
    }
    void Bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
    }
    void Unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void SetData(GLsizeiptr size, const void* data, GLenum usage) {
        Bind();
        glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
        Logger::Info("[UniformBuffer] Data set (size: " + std::to_string(size) + " bytes).");
        Unbind();
    }
    void BindToPoint(GLuint bindingPoint) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ID);
        Logger::Debug("[UniformBuffer] UBO bound to binding point " + std::to_string(bindingPoint));
    }
};

#endif // UNIFORMBUFFER_H
