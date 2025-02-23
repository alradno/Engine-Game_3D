#pragma once

#include <glad/glad.h>
#include "utils/Logger.h"

class UniformBuffer {
public:
    unsigned int ID = 0;
    
    UniformBuffer() {
        glGenBuffers(1, &ID);
        Logger::ThrottledLog("UniformBuffer_Generated", LogLevel::DEBUG, 
                             "[UniformBuffer] Generated ID: " + std::to_string(ID), 0.5);
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
        Logger::ThrottledLog("UniformBuffer_BindToPoint", LogLevel::DEBUG, 
                             "[UniformBuffer] Bound to point " + std::to_string(bindingPoint), 0.5);
    }
};