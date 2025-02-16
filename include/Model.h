#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include "ModelLoader.h"
#include "Submesh.h"
#include "Material.h"
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
// If the texture path is relative, this function prepends the model's base directory.
inline std::string GetTexturePath(aiMaterial* material, aiTextureType type, const std::string& baseDir) {
    aiString str;
    // Attempt to get the texture path for the specified type.
    if (material->GetTexture(type, 0, &str) != AI_SUCCESS) {
        // For albedo textures, try DIFFUSE if BASE_COLOR is not found.
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
    // If the texture path is not absolute, convert it using the model's base directory.
    if (!fsPath.is_absolute()) {
        std::string absPath = baseDir + "/" + relPath;
        Logger::Debug("[GetTexturePath] Converted to absolute path using model directory: " + absPath);
        return absPath;
    }
    return relPath;
}

// The Model class is responsible for loading a model from a file using Assimp,
// processing its meshes and materials, and providing a Draw() method to render it.
class Model {
public:
    // A collection of submeshes that make up the model.
    std::vector<Submesh> submeshes;
    
    // Constructor: Loads a model from the specified file path.
    Model(const std::string &path) {
        Logger::Info("[Model::Model] Loading model from path: " + path);
        loadModel(path);
    }
    
    // Draws the model by drawing each of its submeshes.
    void Draw();
    
private:
    // Loads a model from the specified file path using Assimp.
    // This function processes the scene and populates the submeshes vector.
    void loadModel(const std::string &path);
};

#endif