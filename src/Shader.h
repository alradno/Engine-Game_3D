#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Logger.h"  // Include Logger for detailed logging

class Shader {
public:
    unsigned int ID;
    
    // Constructor initializes the shader ID to 0.
    Shader() : ID(0) { }
    
    // Compiles the shader from given vertex and fragment shader file paths.
    void Compile(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        
        // Enable exceptions on ifstream objects for robust error handling.
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            Logger::Debug("[Shader::Compile] Opening vertex shader file: " + std::string(vertexPath));
            vShaderFile.open(vertexPath);
            Logger::Debug("[Shader::Compile] Opening fragment shader file: " + std::string(fragmentPath));
            fShaderFile.open(fragmentPath);
            
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            Logger::Debug("[Shader::Compile] Vertex shader code length: " + std::to_string(vertexCode.size()) + " bytes");
            Logger::Debug("[Shader::Compile] Fragment shader code length: " + std::to_string(fragmentCode.size()) + " bytes");
            
            vShaderFile.close();
            fShaderFile.close();
        } catch (std::ifstream::failure&) {
            Logger::Error("[Shader::Compile] ERROR: Failed to read shader file: " + std::string(vertexPath) + " or " + std::string(fragmentPath));
        }
        
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        
        unsigned int vertex, fragment;
        int success;
        char infoLog[512];
        
        // Compile vertex shader.
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            Logger::Error(std::string("[Shader::Compile] ERROR: Vertex shader compilation failed\n") + infoLog);
        } else {
            Logger::Info("[Shader::Compile] Vertex shader compiled successfully.");
        }
        
        // Compile fragment shader.
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            Logger::Error(std::string("[Shader::Compile] ERROR: Fragment shader compilation failed\n") + infoLog);
        } else {
            Logger::Info("[Shader::Compile] Fragment shader compiled successfully.");
        }
        
        // Create shader program, attach shaders, and link the program.
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            Logger::Error(std::string("[Shader::Compile] ERROR: Shader program linking failed\n") + infoLog);
        } else {
            Logger::Info("[Shader::Compile] Shader program linked successfully. Program ID: " + std::to_string(ID));
        }
        
        // Once linked, the individual shader objects can be deleted.
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    // Activates the shader program.
    void Use() {
        glUseProgram(ID);
    }
};

#endif