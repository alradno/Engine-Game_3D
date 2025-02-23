// ResourceManager.h
#pragma once
#include <map>
#include <string>
#include <memory>
#include <future>
#include "renderer/Shader.h"
#include "renderer/Texture2D.h"
#include "renderer/Model.h"
#include "engine/Config.h"

class ResourceManager {
public:
    static void SetConfig(const Config& config) { m_Config = config; }

    static std::shared_ptr<Shader> LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);
    static std::shared_ptr<Shader> LoadShaderWithFragment(const std::string& fragmentShaderName, const std::string& key);

    static std::shared_ptr<Texture2D> LoadTexture(const char* file, bool alpha, std::string name);
    static std::shared_ptr<Model> LoadModel(const char* file, std::string name);

    // Funciones asíncronas
    static std::future<std::shared_ptr<Texture2D>> LoadTextureAsync(const char* file, bool alpha, std::string name);
    static std::future<std::shared_ptr<Model>> LoadModelAsync(const char* file, std::string name);

    static std::shared_ptr<Shader> GetShader(const std::string& name);
    static std::shared_ptr<Texture2D> GetTexture(const std::string& name);
    static std::shared_ptr<Model> GetModel(const std::string& name);

    static void Clear();

private:
    ResourceManager() = default;
    static Config m_Config;
    static std::map<std::string, std::shared_ptr<Shader>> Shaders;
    static std::map<std::string, std::shared_ptr<Texture2D>> Textures;
    static std::map<std::string, std::shared_ptr<Model>> Models;
};