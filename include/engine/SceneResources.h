#pragma once
#include <map>
#include <memory>
#include <string>
#include "renderer/Shader.h"
#include "renderer/Texture2D.h"
#include "renderer/Model.h"

/**
 * @brief Clase para gestionar recursos exclusivos de una escena.
 *
 * Permite cargar shaders, texturas y modelos de forma local, sin compartirlos con otras escenas.
 */
class SceneResources {
public:
    std::shared_ptr<Shader> LoadShader(const char* vShaderFile, const char* fShaderFile, const std::string& name);
    std::shared_ptr<Texture2D> LoadTexture(const char* file, bool alpha, const std::string& name);
    std::shared_ptr<Model> LoadModel(const char* file, const std::string& name);
    
    std::shared_ptr<Shader> GetShader(const std::string& name);
    std::shared_ptr<Texture2D> GetTexture(const std::string& name);
    std::shared_ptr<Model> GetModel(const std::string& name);
    
    // Libera todos los recursos cargados en esta escena.
    void Clear();
    
private:
    std::map<std::string, std::shared_ptr<Shader>> shaders;
    std::map<std::string, std::shared_ptr<Texture2D>> textures;
    std::map<std::string, std::shared_ptr<Model>> models;
};