#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <string>
#include <memory>
#include <future>
#include "Shader.h"
#include "Texture2D.h"

// Forward declaration to avoid circular dependency.
class Model;

class ResourceManager {
public:
    static std::shared_ptr<Shader> LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);
    static std::shared_ptr<Shader> GetShader(std::string name);
    
    static std::shared_ptr<Texture2D> LoadTexture(const char* file, bool alpha, std::string name);
    static std::shared_ptr<Texture2D> GetTexture(std::string name);
    
    static std::shared_ptr<Model> LoadModel(const char* file, std::string name);
    static std::shared_ptr<Model> GetModel(std::string name);
    
    static void Clear();
    
    static std::future<std::shared_ptr<Model>> LoadModelAsync(const char* file, std::string name);
    
private:
    ResourceManager() { }
    static std::map<std::string, std::shared_ptr<Shader>> Shaders;
    static std::map<std::string, std::shared_ptr<Texture2D>> Textures;
    static std::map<std::string, std::shared_ptr<Model>> Models;
};

#endif
