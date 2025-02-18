#pragma once
#include <map>
#include <string>
#include <memory>
#include <future>
#include "Shader.h"
#include "Texture2D.h"
#include "Model.h"
#include "Config.h"

class ResourceManager {
public:
    static void SetConfig(const Config& config) { m_Config = config; }

    // Carga un shader dado un vertex y un fragment shader, con un nombre clave.
    static std::shared_ptr<Shader> LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);
    
    // Carga un shader usando el vertex shader global (definido en config) y un fragment shader dado (sin extensión).
    static std::shared_ptr<Shader> LoadShaderWithFragment(const std::string& fragmentShaderName, const std::string& key);

    static std::shared_ptr<Texture2D> LoadTexture(const char* file, bool alpha, std::string name);
    static std::shared_ptr<Model> LoadModel(const char* file, std::string name);

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