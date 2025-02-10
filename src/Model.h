#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include "ModelLoader.h"  // Defines Vertex and processNode
#include "Submesh.h"      // Defines Submesh
#include "Material.h"     // Defines Material
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <iostream>
#include "ResourceManager.h"
#include <filesystem>
#include "Logger.h"

// Global project root (if needed elsewhere)
extern std::string gProjectRoot;

// Inline helper function to get the texture path from an aiMaterial.
// If the path is relative, it prepends the model's base directory.
inline std::string GetTexturePath(aiMaterial* material, aiTextureType type, const std::string& baseDir) {
    aiString str;
    if (material->GetTexture(type, 0, &str) != AI_SUCCESS) {
        if (type == aiTextureType_BASE_COLOR) {
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) != AI_SUCCESS)
                return "";
        } else {
            return "";
        }
    }
    std::string relPath(str.C_Str());
    Logger::Debug("[GetTexturePath] Original path from material: " + relPath);
    std::filesystem::path fsPath(relPath);
    if (!fsPath.is_absolute()) {
        std::string absPath = baseDir + "/" + relPath;
        Logger::Debug("[GetTexturePath] Converted to absolute path using model directory: " + absPath);
        return absPath;
    }
    return relPath;
}

class Model {
public:
    std::vector<Submesh> submeshes;
    
    Model(const std::string &path) {
        loadModel(path);
    }
    
    void Draw();
    
private:
    void loadModel(const std::string &path);
};

#endif
