#pragma once

#include <string>
#include <vector>
#include "ModelLoader.h"
#include "Submesh.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "ResourceManager.h"
#include <filesystem>
#include "Logger.h"

// Variable global (si es requerida en otras partes)
extern std::string gProjectRoot;

inline std::string GetTexturePath(aiMaterial* material, aiTextureType type, const std::string& baseDir) {
    aiString str;
    if (material->GetTexture(type, 0, &str) != AI_SUCCESS) {
        if (type == aiTextureType_BASE_COLOR)
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) != AI_SUCCESS)
                return "";
        else
            return "";
    }
    std::string relPath(str.C_Str());
    Logger::Debug("[Model] Texture original: " + relPath);
    std::filesystem::path fsPath(relPath);
    if (!fsPath.is_absolute())
        return baseDir + "/" + relPath;
    return relPath;
}

class Model {
public:
    std::vector<Submesh> submeshes;
    
    explicit Model(const std::string &path) {
        Logger::Info("[Model] Loading from: " + path);
        loadModel(path);
    }
    
    void Draw();
    
private:
    void loadModel(const std::string &path);
};