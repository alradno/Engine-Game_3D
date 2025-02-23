// ResourceManager.cpp
#include "renderer/ResourceManager.h"
#include "utils/FileUtils.h"
#include "utils/Logger.h"
#include <filesystem>
#include "utils/GLDebug.h"
#include <cassert>
#include <future>

Config ResourceManager::m_Config;
std::map<std::string, std::shared_ptr<Shader>> ResourceManager::Shaders;
std::map<std::string, std::shared_ptr<Texture2D>> ResourceManager::Textures;
std::map<std::string, std::shared_ptr<Model>> ResourceManager::Models;

std::shared_ptr<Shader> ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name)
{
    try
    {
        std::string vertexPath = FileUtils::NormalizePath(vShaderFile);
        std::filesystem::path vp(vertexPath);
        if (!vp.is_absolute())
        {
            vertexPath = FileUtils::ResolvePath(m_Config.projectRoot + m_Config.shaders, vertexPath);
        }
        std::string fragmentPath = FileUtils::NormalizePath(fShaderFile);
        std::filesystem::path fp(fragmentPath);
        if (!fp.is_absolute())
        {
            fragmentPath = FileUtils::ResolvePath(m_Config.projectRoot + m_Config.shaders, fragmentPath);
        }

        auto shader = std::make_shared<Shader>();
        shader->Compile(vertexPath.c_str(), fragmentPath.c_str());
        Shaders[name] = shader;
        Logger::Info("[ResourceManager] Shader loaded: " + name + " (ID: " + std::to_string(shader->ID) + ")");
        return shader;
    }
    catch (const std::exception &e)
    {
        Logger::Error("[ResourceManager] Exception while loading shader: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Shader> ResourceManager::LoadShaderWithFragment(const std::string &fragmentShaderName, const std::string &key)
{
    try
    {
        std::string vertexShaderFile = m_Config.vertexShader;
        std::string fragmentShaderFile = fragmentShaderName;
        vertexShaderFile = FileUtils::ResolvePath(m_Config.projectRoot + m_Config.shaders, vertexShaderFile);
        fragmentShaderFile = FileUtils::ResolvePath(m_Config.projectRoot + m_Config.shaders, fragmentShaderFile);
        return LoadShader(vertexShaderFile.c_str(), fragmentShaderFile.c_str(), key);
    }
    catch (const std::exception &e)
    {
        Logger::Error("[ResourceManager] Exception in LoadShaderWithFragment: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    try
    {
        std::string filePath = FileUtils::NormalizePath(file);
        std::filesystem::path p(filePath);
        if (!p.is_absolute())
        {
            std::string normalizedFilePath = filePath;
            std::string prefix1 = m_Config.assets;
            std::string prefix2 = "./" + m_Config.assets;
            if (normalizedFilePath.find(prefix1) == 0 || normalizedFilePath.find(prefix2) == 0)
            {
                if (m_Config.projectRoot != "./")
                {
                    filePath = FileUtils::ResolvePath(m_Config.projectRoot, normalizedFilePath);
                }
            }
            else
            {
                filePath = FileUtils::ResolvePath(m_Config.projectRoot + m_Config.assets, filePath);
            }
        }

        Logger::Debug("[ResourceManager] Loading image from: " + filePath);

        auto texture = std::make_shared<Texture2D>();

        int width, height, nrChannels;
        unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, alpha ? 4 : 3);
        assert(data && "Error: Image data is null after stbi_load.");
        if (data)
        {
            Logger::Debug("[ResourceManager] Image loaded: " + filePath +
                          " (" + std::to_string(width) + "x" + std::to_string(height) +
                          ", channels: " + std::to_string(nrChannels) + ")");
            FileUtils::ImageData imgData;
            imgData.data = data;
            imgData.width = width;
            imgData.height = height;
            imgData.channels = nrChannels;
            texture->GenerateFromData(imgData, alpha);
        }
        else
        {
            Logger::Error("[ResourceManager] Failed to load image: " + filePath);
            return nullptr;
        }

        Textures[name] = texture;
        Logger::Info("[ResourceManager] Texture loaded: " + name + " (" + filePath + ")");
        return texture;
    }
    catch (const std::exception &e)
    {
        Logger::Error("[ResourceManager] Exception while loading texture: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Model> ResourceManager::LoadModel(const char *file, std::string name)
{
    try
    {
        std::string filePath = FileUtils::NormalizePath(file);
        std::filesystem::path p(filePath);
        if (!p.is_absolute())
        {
            filePath = FileUtils::ResolvePath(m_Config.projectRoot + m_Config.assets, filePath);
        }
        auto model = std::make_shared<Model>(filePath);
        Models[name] = model;
        Logger::Info("[ResourceManager] Model loaded: " + name);
        return model;
    }
    catch (const std::exception &e)
    {
        Logger::Error("[ResourceManager] Exception while loading model: " + std::string(e.what()));
        return nullptr;
    }
}

// Funciones asíncronas

std::future<std::shared_ptr<Texture2D>> ResourceManager::LoadTextureAsync(const char *file, bool alpha, std::string name) {
    return std::async(std::launch::async, [file, alpha, name]() {
        return LoadTexture(file, alpha, name);
    });
}

std::future<std::shared_ptr<Model>> ResourceManager::LoadModelAsync(const char *file, std::string name) {
    return std::async(std::launch::async, [file, name]() {
        return LoadModel(file, name);
    });
}

std::shared_ptr<Shader> ResourceManager::GetShader(const std::string &name) {
    return Shaders[name];
}

std::shared_ptr<Texture2D> ResourceManager::GetTexture(const std::string &name) {
    return Textures[name];
}

std::shared_ptr<Model> ResourceManager::GetModel(const std::string &name) {
    return Models[name];
}

void ResourceManager::Clear() {
    Logger::Info("[ResourceManager] Clearing all resources.");
    for (auto &iter : Shaders)
        GLCall(glDeleteProgram(iter.second->ID));
    Shaders.clear();
    for (auto &iter : Textures)
        GLCall(glDeleteTextures(1, &iter.second->ID));
    Textures.clear();
    Models.clear();
}