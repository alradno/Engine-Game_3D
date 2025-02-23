#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include "utils/Logger.h"

class Shader {
public:
    unsigned int ID = 0;
    
    Shader() = default;
    
    void Compile(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode, fragmentCode;
        std::ifstream vShaderFile, fShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try {
            Logger::Debug("[Shader] Opening: " + std::string(vertexPath));
            vShaderFile.open(vertexPath);
            Logger::Debug("[Shader] Opening: " + std::string(fragmentPath));
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            vShaderFile.close();
            fShaderFile.close();
        } catch (std::ifstream::failure&) {
            Logger::Error("[Shader] ERROR: Failed to read shader files");
        }
        
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;
        int success;
        char infoLog[512];
        
        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            Logger::Error("[Shader] Vertex compilation failed:\n" + std::string(infoLog));
        } else {
            Logger::Info("[Shader] Vertex shader compiled");
        }
        
        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            Logger::Error("[Shader] Fragment compilation failed:\n" + std::string(infoLog));
        } else {
            Logger::Info("[Shader] Fragment shader compiled");
        }
        
        // Shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            Logger::Error("[Shader] Linking failed:\n" + std::string(infoLog));
        } else {
            Logger::Info("[Shader] Program linked. ID: " + std::to_string(ID));
        }
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void Use() {
        glUseProgram(ID);
    }
};