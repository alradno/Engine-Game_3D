#include "Model.h"
#include "Submesh.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <sstream>
#include <filesystem>
#include "Logger.h"

void Model::loadModel(const std::string &path) {
    Logger::Info("[Model::loadModel] Starting model load: " + path);
    
    Assimp::Importer importer;
    // Read the model file with triangulation, flipped UVs, and tangent space calculation.
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger::Error("[Model::loadModel] ERROR: Assimp failed to open file: " + path +
                      "\nReason: " + importer.GetErrorString());
        return;
    }
    
    // Extract the model's base directory for locating textures.
    std::filesystem::path modelFilePath(path);
    std::string modelDir = modelFilePath.parent_path().generic_string();
    Logger::Info("[Model::loadModel] Model base directory: " + modelDir);
    
    // Process each mesh in the scene.
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        Submesh submesh;
        Logger::Info("[Model::loadModel] Processing mesh " + std::to_string(i) +
                     " with " + std::to_string(mesh->mNumVertices) + " vertices.");
        
        // Process mesh vertices.
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[j].x,
                                        mesh->mVertices[j].y,
                                        mesh->mVertices[j].z);
            if (mesh->HasNormals()) {
                vertex.Normal = glm::normalize(glm::vec3(mesh->mNormals[j].x,
                                                         mesh->mNormals[j].y,
                                                         mesh->mNormals[j].z));
            } else {
                vertex.Normal = glm::vec3(0.0f);
            }
            if (mesh->HasTextureCoords(0)) {
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][j].x,
                                             mesh->mTextureCoords[0][j].y);
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            if (mesh->HasTangentsAndBitangents()) {
                vertex.Tangent = glm::normalize(glm::vec3(mesh->mTangents[j].x,
                                                          mesh->mTangents[j].y,
                                                          mesh->mTangents[j].z));
            } else {
                vertex.Tangent = glm::vec3(0.0f);
            }
            submesh.vertices.push_back(vertex);
        }
        
        // Process mesh faces (indices).
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                submesh.indices.push_back(face.mIndices[k]);
            }
        }
        
        Logger::Info("[Model::loadModel] Mesh " + std::to_string(i) +
                     " has " + std::to_string(mesh->mNumFaces) + " faces.");
        
        // Process materials and textures.
        if (scene->HasMaterials()) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            
            // Attempt to get the albedo texture path (first trying BASE_COLOR, then DIFFUSE).
            std::string albedoPath = GetTexturePath(material, aiTextureType_BASE_COLOR, modelDir);
            if (albedoPath.empty())
                albedoPath = GetTexturePath(material, aiTextureType_DIFFUSE, modelDir);
            if (!albedoPath.empty()) {
                Logger::Info("[Model::loadModel] Loading albedo texture: " + albedoPath);
                submesh.material.albedo = ResourceManager::LoadTexture(albedoPath.c_str(), true, albedoPath);
            } else {
                Logger::Warning("[Model::loadModel] No albedo texture path found for mesh " + std::to_string(i));
            }
            
            // Attempt to load the normal texture.
            std::string normalPath = GetTexturePath(material, aiTextureType_NORMALS, modelDir);
            if (!normalPath.empty()) {
                Logger::Info("[Model::loadModel] Loading normal texture: " + normalPath);
                submesh.material.normal = ResourceManager::LoadTexture(normalPath.c_str(), false, normalPath);
            } else {
                Logger::Warning("[Model::loadModel] No normal texture path found for mesh " + std::to_string(i));
            }
            
            // Attempt to load the metallic/roughness texture (try UNKNOWN first, then SPECULAR).
            std::string mrPath = GetTexturePath(material, aiTextureType_UNKNOWN, modelDir);
            if (mrPath.empty())
                mrPath = GetTexturePath(material, aiTextureType_SPECULAR, modelDir);
            if (!mrPath.empty()) {
                Logger::Info("[Model::loadModel] Loading metallic/roughness texture: " + mrPath);
                submesh.material.metallicRoughness = ResourceManager::LoadTexture(mrPath.c_str(), false, mrPath);
            } else {
                Logger::Warning("[Model::loadModel] No metallic/roughness texture path found for mesh " + std::to_string(i));
            }
        }
        
        // Setup the mesh (upload buffers, etc.).
        submesh.setupMesh();
        Logger::Info("[Model::loadModel] Mesh " + std::to_string(i) + " loaded: " +
                     std::to_string(submesh.vertices.size()) + " vertices, " +
                     std::to_string(submesh.indices.size()) + " indices.");
        submeshes.push_back(submesh);
    }
    
    Logger::Info("[Model::loadModel] Total submeshes loaded: " + std::to_string(submeshes.size()));
}

void Model::Draw() {
    Logger::Info("[Model::Draw] Drawing model with " + std::to_string(submeshes.size()) + " submeshes.");
    
    // Iterate over submeshes and draw each one if it has valid data.
    for (size_t i = 0; i < submeshes.size(); ++i) {
        if (submeshes[i].vertices.empty() || submeshes[i].indices.empty()) {
            Logger::Warning("[Model::Draw] Submesh " + std::to_string(i) + " has invalid data. Skipping draw.");
            continue;
        }
        submeshes[i].Draw();
    }
}