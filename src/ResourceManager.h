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

// The ResourceManager class is responsible for loading, caching, and providing access to various resources
// such as shaders, textures, and models. This avoids redundant loading and improves performance.
// Detailed logging is performed in the corresponding source file to trace resource management events.
class ResourceManager {
public:
    // Loads a shader from the specified vertex and fragment shader files, and stores it under the given name.
    // Returns a shared pointer to the loaded Shader.
    static std::shared_ptr<Shader> LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);
    
    // Retrieves a previously loaded shader by its name.
    static std::shared_ptr<Shader> GetShader(std::string name);
    
    // Loads a texture from a file. If 'alpha' is true, the texture is loaded with an alpha channel.
    // The texture is stored under the provided name.
    static std::shared_ptr<Texture2D> LoadTexture(const char* file, bool alpha, std::string name);
    
    // Retrieves a previously loaded texture by its name.
    static std::shared_ptr<Texture2D> GetTexture(std::string name);
    
    // Loads a 3D model from a file and stores it under the specified name.
    static std::shared_ptr<Model> LoadModel(const char* file, std::string name);
    
    // Retrieves a previously loaded model by its name.
    static std::shared_ptr<Model> GetModel(std::string name);
    
    // Clears all loaded resources (shaders, textures, and models). Note that models are automatically released via shared_ptr.
    static void Clear();
    
    // Asynchronously loads a 3D model from a file. Returns a future that holds a shared pointer to the loaded Model.
    static std::future<std::shared_ptr<Model>> LoadModelAsync(const char* file, std::string name);
    
private:
    // Private constructor to prevent instantiation.
    ResourceManager() { }
    
    // Static maps for caching loaded resources.
    static std::map<std::string, std::shared_ptr<Shader>> Shaders;
    static std::map<std::string, std::shared_ptr<Texture2D>> Textures;
    static std::map<std::string, std::shared_ptr<Model>> Models;
};

#endif // RESOURCEMANAGER_H