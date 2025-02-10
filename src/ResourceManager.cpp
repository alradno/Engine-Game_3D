#include "ResourceManager.h"
#include "Model.h"          // Full definition of Model
#include "FileUtils.h"      // For LoadImageData and NormalizePath
#include "Logger.h"
#include <iostream>
#include <future>
#include <mutex>

// Global caches for resources
std::map<std::string, std::shared_ptr<Shader>>    ResourceManager::Shaders;
std::map<std::string, std::shared_ptr<Texture2D>> ResourceManager::Textures;
std::map<std::string, std::shared_ptr<Model>>     ResourceManager::Models;

// Mutex for thread-safe texture loading.
std::mutex textureMutex;

std::shared_ptr<Shader> ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name) {
    std::string vertexPath = FileUtils::NormalizePath(vShaderFile);
    std::string fragmentPath = FileUtils::NormalizePath(fShaderFile);
    Logger::Info("[ResourceManager::LoadShader] Loading shader: " + vertexPath + ", " + fragmentPath + " (name: " + name + ")");
    std::shared_ptr<Shader> shader = std::make_shared<Shader>();
    shader->Compile(vertexPath.c_str(), fragmentPath.c_str());
    if (shader->ID == 0) {
        Logger::Error("[ResourceManager::LoadShader] Shader " + name + " did not compile correctly.");
    } else {
        Logger::Info("[ResourceManager::LoadShader] Shader loaded successfully. Program ID: " + std::to_string(shader->ID));
    }
    Shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const char* file, bool alpha, std::string name) {
    std::string filePath = FileUtils::NormalizePath(file);
    
    {
        std::lock_guard<std::mutex> lock(textureMutex);
        if (Textures.find(filePath) != Textures.end()) {
            Logger::Debug("[ResourceManager::LoadTexture] Texture already loaded: " + filePath);
            return Textures[filePath];
        }
    }
    Logger::Info("[ResourceManager::LoadTexture] Loading texture from file: " + filePath + " (name: " + name + ")");
    
    auto futureImageData = std::async(std::launch::async, [filePath, alpha]() -> FileUtils::ImageData {
        return FileUtils::LoadImageData(filePath, alpha);
    });
    FileUtils::ImageData imgData = futureImageData.get();
    
    std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
    if (filePath.find("baseColor") != std::string::npos) {
        if (alpha) {
            texture->Internal_Format = GL_SRGB_ALPHA;
            texture->Image_Format = GL_SRGB_ALPHA;
        } else {
            texture->Internal_Format = GL_SRGB;
            texture->Image_Format = GL_SRGB;
        }
    } else {
        if (alpha) {
            texture->Internal_Format = GL_RGBA;
            texture->Image_Format = GL_RGBA;
        } else {
            texture->Internal_Format = GL_RGB;
            texture->Image_Format = GL_RGB;
        }
    }
    
    texture->GenerateFromData(imgData, alpha);
    if (texture->ID == 0) {
        Logger::Error("[ResourceManager::LoadTexture] Texture " + filePath + " failed to load.");
    } else {
        Logger::Info("[ResourceManager::LoadTexture] Texture loaded successfully. Texture ID: " + std::to_string(texture->ID));
    }
    {
        std::lock_guard<std::mutex> lock(textureMutex);
        Textures[filePath] = texture;
    }
    return texture;
}

std::shared_ptr<Texture2D> ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

std::shared_ptr<Model> ResourceManager::LoadModel(const char* file, std::string name) {
    std::string filePath = FileUtils::NormalizePath(file);
    Logger::Info("[ResourceManager::LoadModel] Loading model: " + filePath + " (name: " + name + ")");
    std::shared_ptr<Model> model = std::make_shared<Model>(filePath);
    if (model->submeshes.empty()) {
        Logger::Error("[ResourceManager::LoadModel] Model " + filePath + " did not load properly (empty submeshes).");
    } else {
        Logger::Info("[ResourceManager::LoadModel] Model loaded successfully. Number of submeshes: " + std::to_string(model->submeshes.size()));
    }
    Models[name] = model;
    return model;
}

std::shared_ptr<Model> ResourceManager::GetModel(std::string name) {
    return Models[name];
}

void ResourceManager::Clear() {
    for (auto iter : Shaders) {
        Logger::Info("[ResourceManager::Clear] Deleting shader: " + iter.first);
        glDeleteProgram(iter.second->ID);
    }
    for (auto iter : Textures) {
        Logger::Info("[ResourceManager::Clear] Deleting texture: " + iter.first);
        glDeleteTextures(1, &iter.second->ID);
    }
    // Los modelos se liberan automáticamente con shared_ptr.
}

std::future<std::shared_ptr<Model>> ResourceManager::LoadModelAsync(const char* file, std::string name) {
    return std::async(std::launch::async, [file, name]() -> std::shared_ptr<Model> {
        return ResourceManager::LoadModel(file, name);
    });
}
