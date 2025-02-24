#include "engine/SceneResources.h"
#include "utils/FileUtils.h"
#include "utils/Logger.h"
#include <filesystem>
#include "utils/GLDebug.h"
#include "stb_image.h" // Asegúrate de incluir stb_image

std::shared_ptr<Shader> SceneResources::LoadShader(const char* vShaderFile, const char* fShaderFile, const std::string& name) {
    // Si ya existe, devolver el recurso
    auto it = shaders.find(name);
    if(it != shaders.end())
        return it->second;
    
    try {
        std::string vertexPath = FileUtils::NormalizePath(vShaderFile);
        std::filesystem::path vp(vertexPath);
        if (!vp.is_absolute()) {
            // Suponemos que los shaders se encuentran en "./shaders/"
            vertexPath = FileUtils::ResolvePath("./shaders/", vertexPath);
        }
        std::string fragmentPath = FileUtils::NormalizePath(fShaderFile);
        std::filesystem::path fp(fragmentPath);
        if (!fp.is_absolute()) {
            fragmentPath = FileUtils::ResolvePath("./shaders/", fragmentPath);
        }
        
        auto shader = std::make_shared<Shader>();
        shader->Compile(vertexPath.c_str(), fragmentPath.c_str());
        shaders[name] = shader;
        Logger::Info("[SceneResources] Shader loaded: " + name + " (ID: " + std::to_string(shader->ID) + ")");
        return shader;
    } catch (const std::exception& e) {
        Logger::Error("[SceneResources] Exception while loading shader: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Texture2D> SceneResources::LoadTexture(const char* file, bool alpha, const std::string& name) {
    // Si ya está cargada, devolverla
    auto it = textures.find(name);
    if(it != textures.end())
        return textures[name];
    
    try {
        std::string filePath = FileUtils::NormalizePath(file);
        std::filesystem::path p(filePath);
        if (!p.is_absolute()) {
            // Suponemos que las texturas se encuentran en "./assets/"
            filePath = FileUtils::ResolvePath("./assets/", filePath);
        }
        
        Logger::Debug("[SceneResources] Loading texture from: " + filePath);
        auto texture = std::make_shared<Texture2D>();
        
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, alpha ? 4 : 3);
        if (!data) {
            Logger::Error("[SceneResources] Failed to load image: " + filePath);
            return nullptr;
        }
        
        FileUtils::ImageData imgData;
        imgData.data = data;
        imgData.width = width;
        imgData.height = height;
        imgData.channels = nrChannels;
        texture->GenerateFromData(imgData, alpha);
        textures[name] = texture;
        Logger::Info("[SceneResources] Texture loaded: " + name + " (" + filePath + ")");
        return texture;
    } catch (const std::exception& e) {
        Logger::Error("[SceneResources] Exception while loading texture: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Model> SceneResources::LoadModel(const char* file, const std::string& name) {
    // Si ya existe, devolverlo
    auto it = models.find(name);
    if(it != models.end())
        return models[name];
    
    try {
        std::string filePath = FileUtils::NormalizePath(file);
        std::filesystem::path p(filePath);
        if (!p.is_absolute()) {
            filePath = FileUtils::ResolvePath("./assets/", filePath);
        }
        auto model = std::make_shared<Model>(filePath);
        models[name] = model;
        Logger::Info("[SceneResources] Model loaded: " + name);
        return model;
    } catch (const std::exception& e) {
        Logger::Error("[SceneResources] Exception while loading model: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Shader> SceneResources::GetShader(const std::string& name) {
    if(shaders.find(name) != shaders.end())
        return shaders[name];
    return nullptr;
}

std::shared_ptr<Texture2D> SceneResources::GetTexture(const std::string& name) {
    if(textures.find(name) != textures.end())
        return textures[name];
    return nullptr;
}

std::shared_ptr<Model> SceneResources::GetModel(const std::string& name) {
    if(models.find(name) != models.end())
        return models[name];
    return nullptr;
}

void SceneResources::Clear() {
    for(auto &iter : shaders) {
        GLCall(glDeleteProgram(iter.second->ID));
    }
    shaders.clear();
    
    for(auto &iter : textures) {
        GLCall(glDeleteTextures(1, &iter.second->ID));
    }
    textures.clear();
    
    models.clear();
    
    Logger::Info("[SceneResources] Cleared all scene resources.");
}