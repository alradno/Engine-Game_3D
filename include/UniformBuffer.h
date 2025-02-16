#pragma once

#include <glad/glad.h>
#include "Logger.h"

class UniformBuffer {
public:
    unsigned int ID = 0;
    
    UniformBuffer() {
        glGenBuffers(1, &ID);
        Logger::Debug("[UniformBuffer] Generated ID: " + std::to_string(ID));
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
        Logger::Info("[UniformBuffer] Data set (" + std::to_string(size) + " bytes)");
        Unbind();
    }
    
    void BindToPoint(GLuint bindingPoint) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ID);
        Logger::Debug("[UniformBuffer] Bound to point " + std::to_string(bindingPoint));
    }
};