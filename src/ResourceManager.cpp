/**
 * @file ResourceManager.cpp
 * @brief Implementation of the ResourceManager for loading, caching, and managing shaders, textures, and models.
 */

#include "ResourceManager.h"
#include "Model.h"
#include "FileUtils.h"
#include "Logger.h"
#include "ShaderFactory.h"
#include <iostream>
#include <future>
#include <mutex>

std::map<std::string, std::shared_ptr<Shader>> ResourceManager::Shaders;
std::map<std::string, std::shared_ptr<Texture2D>> ResourceManager::Textures;
std::map<std::string, std::shared_ptr<Model>> ResourceManager::Models;

std::mutex textureMutex;

std::shared_ptr<Shader> ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name)
{
    std::string vertexPath = FileUtils::NormalizePath(vShaderFile);
    std::string fragmentPath = FileUtils::NormalizePath(fShaderFile);
    Logger::Info("[ResourceManager] Loading shader: " + vertexPath + ", " + fragmentPath + " (name: " + name + ")");

    ShaderFactory factory;
    auto shader = factory.CreateShader(vertexPath, fragmentPath);

    if (shader->ID == 0)
        Logger::Error("[ResourceManager] Shader '" + name + "' failed to compile.");
    else
        Logger::Info("[ResourceManager] Shader loaded. ID: " + std::to_string(shader->ID));

    Shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    std::string filePath = FileUtils::NormalizePath(file);

    {
        std::lock_guard<std::mutex> lock(textureMutex);
        if (Textures.find(filePath) != Textures.end())
        {
            Logger::Debug("[ResourceManager] Texture already loaded: " + filePath);
            return Textures[filePath];
        }
    }

    Logger::Info("[ResourceManager] Loading texture: " + filePath + " (name: " + name + ")");

    auto futureImageData = std::async(std::launch::async, [filePath, alpha]() -> FileUtils::ImageData
                                      { return FileUtils::LoadImageData(filePath, alpha); });
    FileUtils::ImageData imgData = futureImageData.get();

    auto texture = std::make_shared<Texture2D>();
    if (filePath.find("baseColor") != std::string::npos)
    {
        if (alpha)
        {
            texture->Internal_Format = GL_SRGB_ALPHA;
            texture->Image_Format = GL_SRGB_ALPHA;
        }
        else
        {
            texture->Internal_Format = GL_SRGB;
            texture->Image_Format = GL_SRGB;
        }
    }
    else
    {
        if (alpha)
        {
            texture->Internal_Format = GL_RGBA;
            texture->Image_Format = GL_RGBA;
        }
        else
        {
            texture->Internal_Format = GL_RGB;
            texture->Image_Format = GL_RGB;
        }
    }

    texture->GenerateFromData(imgData, alpha);

    if (texture->ID == 0)
        Logger::Error("[ResourceManager] Texture '" + filePath + "' failed to load.");
    else
        Logger::Info("[ResourceManager] Texture loaded. ID: " + std::to_string(texture->ID));

    {
        std::lock_guard<std::mutex> lock(textureMutex);
        Textures[filePath] = texture;
    }
    return texture;
}

std::shared_ptr<Texture2D> ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

std::shared_ptr<Model> ResourceManager::LoadModel(const char *file, std::string name)
{
    std::string filePath = FileUtils::NormalizePath(file);
    Logger::Info("[ResourceManager] Loading model: " + filePath + " (name: " + name + ")");

    auto model = std::make_shared<Model>(filePath);
    if (model->submeshes.empty())
        Logger::Error("[ResourceManager] Model '" + filePath + "' loaded with no submeshes.");
    else
        Logger::Info("[ResourceManager] Model loaded. Submeshes: " + std::to_string(model->submeshes.size()));

    Models[name] = model;
    return model;
}

std::shared_ptr<Model> ResourceManager::GetModel(std::string name)
{
    return Models[name];
}

void ResourceManager::Clear()
{
    Logger::Info("[ResourceManager] Clearing all loaded resources.");
    for (const auto &iter : Shaders)
    {
        Logger::Info("[ResourceManager] Deleting shader: " + iter.first);
        glDeleteProgram(iter.second->ID);
    }
    for (const auto &iter : Textures)
    {
        Logger::Info("[ResourceManager] Deleting texture: " + iter.first);
        glDeleteTextures(1, &iter.second->ID);
    }
    // Models are automatically released by shared_ptr.
}

std::future<std::shared_ptr<Model>> ResourceManager::LoadModelAsync(const char *file, std::string name)
{
    Logger::Info("[ResourceManager] Asynchronously loading model: " + std::string(file) + " (name: " + name + ")");
    return std::async(std::launch::async, [file, name]() -> std::shared_ptr<Model>
                      { return ResourceManager::LoadModel(file, name); });
}