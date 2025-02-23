// Model.cpp
#include "renderer/Model.h"
#include "renderer/Submesh.h"
#include "renderer/Material.h"
#include "renderer/ResourceManager.h" // Para acceder a recursos de materiales
#include "utils/FileUtils.h"
#include "utils/Logger.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <sstream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

/**
 * Carga el material de un mesh usando las propiedades glTF expuestas por Assimp.
 * Se intentan cargar las texturas para:
 *   - Albedo (usando BASE_COLOR o DIFFUSE). Si no existe, se extrae el factor "baseColorFactor".
 *   - Normal (NORMALS)
 *   - MetallicRoughness (UNKNOWN); de no existir, se leen "metallicFactor" y "roughnessFactor".
 *   - Occlusion (AMBIENT)
 *   - Emisivo (EMISSIVE)
 * Se utilizan rutas resueltas de forma genérica.
 */
Material LoadMaterial(aiMaterial* material, const std::string &modelDir) {
    Material mat;
    aiString texPath;
    
    // Albedo: buscar en BASE_COLOR o DIFFUSE
    if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == AI_SUCCESS ||
        material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        std::string texPathStr = texPath.C_Str();
        if (!texPathStr.empty() && texPathStr.front() == '/')
            texPathStr.erase(0, 1);
        std::string fullTexPath = FileUtils::ResolvePath(modelDir, texPathStr);
        Logger::Debug("[LoadMaterial] Loading base color texture from: " + fullTexPath);
        mat.albedo = ResourceManager::LoadTexture(fullTexPath.c_str(), true, fullTexPath);
    } else {
        aiColor4D baseColor;
        if (AI_SUCCESS == aiGetMaterialColor(material, "gltf.pbrMetallicRoughness.baseColorFactor", 0, 0, &baseColor)) {
            mat.baseColorFactor = glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
            Logger::Debug("[LoadMaterial] Using baseColorFactor: " +
                          std::to_string(mat.baseColorFactor.r) + ", " +
                          std::to_string(mat.baseColorFactor.g) + ", " +
                          std::to_string(mat.baseColorFactor.b) + ", " +
                          std::to_string(mat.baseColorFactor.a));
        }
    }
    
    // Normal map
    if (material->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS) {
         std::string texPathStr = texPath.C_Str();
         if (!texPathStr.empty() && texPathStr.front() == '/')
             texPathStr.erase(0, 1);
         std::string fullTexPath = FileUtils::ResolvePath(modelDir, texPathStr);
         Logger::Debug("[LoadMaterial] Loading normal texture from: " + fullTexPath);
         mat.normal = ResourceManager::LoadTexture(fullTexPath.c_str(), true, fullTexPath);
    }
    
    // MetallicRoughness map
    if (material->GetTexture(aiTextureType_UNKNOWN, 0, &texPath) == AI_SUCCESS) {
         std::string texPathStr = texPath.C_Str();
         if (!texPathStr.empty() && texPathStr.front() == '/')
             texPathStr.erase(0, 1);
         std::string fullTexPath = FileUtils::ResolvePath(modelDir, texPathStr);
         Logger::Debug("[LoadMaterial] Loading metallicRoughness texture from: " + fullTexPath);
         mat.metallicRoughness = ResourceManager::LoadTexture(fullTexPath.c_str(), true, fullTexPath);
    } else {
         float metallic = 1.0f, roughness = 1.0f;
         if (AI_SUCCESS == aiGetMaterialFloat(material, "gltf.pbrMetallicRoughness.metallicFactor", 0, 0, &metallic)) {
             mat.metallicFactor = metallic;
             Logger::Debug("[LoadMaterial] Metallic factor: " + std::to_string(metallic));
         }
         if (AI_SUCCESS == aiGetMaterialFloat(material, "gltf.pbrMetallicRoughness.roughnessFactor", 0, 0, &roughness)) {
             mat.roughnessFactor = roughness;
             Logger::Debug("[LoadMaterial] Roughness factor: " + std::to_string(roughness));
         }
    }
    
    // Occlusion map (usualmente en AMBIENT)
    if (material->GetTexture(aiTextureType_AMBIENT, 0, &texPath) == AI_SUCCESS) {
         std::string texPathStr = texPath.C_Str();
         if (!texPathStr.empty() && texPathStr.front() == '/')
             texPathStr.erase(0, 1);
         std::string fullTexPath = FileUtils::ResolvePath(modelDir, texPathStr);
         Logger::Debug("[LoadMaterial] Loading occlusion texture from: " + fullTexPath);
         mat.occlusion = ResourceManager::LoadTexture(fullTexPath.c_str(), false, fullTexPath);
    }
    
    // Emissive map
    if (material->GetTexture(aiTextureType_EMISSIVE, 0, &texPath) == AI_SUCCESS) {
         std::string texPathStr = texPath.C_Str();
         if (!texPathStr.empty() && texPathStr.front() == '/')
             texPathStr.erase(0, 1);
         std::string fullTexPath = FileUtils::ResolvePath(modelDir, texPathStr);
         Logger::Debug("[LoadMaterial] Loading emissive texture from: " + fullTexPath);
         mat.emissive = ResourceManager::LoadTexture(fullTexPath.c_str(), true, fullTexPath);
    }
    
    return mat;
}

Model::Model(const std::string &path) {
    Logger::Info("[Model] Loading from: " + path);
    loadModel(path);
}

void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform, const std::string &modelDir) {
    glm::mat4 nodeTransform = parentTransform * aiMatrix4x4ToGlm(node->mTransformation);
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Submesh submesh;
        Logger::Info("[Model::processNode] Processing mesh from node: " + std::string(node->mName.C_Str()) +
                     ", vertices: " + std::to_string(mesh->mNumVertices));
        
        // Reservar espacio para evitar realineaciones
        submesh.vertices.reserve(mesh->mNumVertices);
        submesh.indices.reserve(mesh->mNumFaces * 3);
        
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(nodeTransform)));
        
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            Vertex vertex;
            glm::vec4 pos = nodeTransform * glm::vec4(mesh->mVertices[j].x,
                                                       mesh->mVertices[j].y,
                                                       mesh->mVertices[j].z,
                                                       1.0f);
            vertex.Position = glm::vec3(pos);
            
            if (mesh->HasNormals()) {
                glm::vec3 norm(mesh->mNormals[j].x,
                               mesh->mNormals[j].y,
                               mesh->mNormals[j].z);
                vertex.Normal = glm::normalize(normalMatrix * norm);
            } else {
                vertex.Normal = glm::vec3(0.0f);
            }
            
            if (mesh->HasTextureCoords(0)) {
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][j].x,
                                             mesh->mTextureCoords[0][j].y);
            } else {
                vertex.TexCoords = glm::vec2(0.0f);
            }
            if (mesh->HasTextureCoords(1)) {
                vertex.TexCoords2 = glm::vec2(mesh->mTextureCoords[1][j].x,
                                              mesh->mTextureCoords[1][j].y);
            } else {
                vertex.TexCoords2 = glm::vec2(0.0f);
            }
            
            if (mesh->HasTangentsAndBitangents()) {
                glm::vec3 tan(mesh->mTangents[j].x,
                              mesh->mTangents[j].y,
                              mesh->mTangents[j].z);
                vertex.Tangent = glm::normalize(normalMatrix * tan);
            } else {
                vertex.Tangent = glm::vec3(0.0f);
            }
            
            submesh.vertices.push_back(vertex);
        }
        
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                submesh.indices.push_back(face.mIndices[k]);
            }
        }
        
        if (scene->HasMaterials()) {
            aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
            submesh.material = LoadMaterial(aiMat, modelDir);
        }
        
        submesh.setupMesh();
        submeshes.push_back(std::move(submesh));
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, nodeTransform, modelDir);
    }
}

void Model::loadModel(const std::string &path) {
    Logger::Info("[Model::loadModel] Starting load: " + path);
    
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger::Error("[Model::loadModel] Failed to load file: " + path + "\nReason: " + importer.GetErrorString());
        return;
    }
    
    std::filesystem::path modelFilePath(path);
    std::string modelDir = modelFilePath.parent_path().generic_string();
    Logger::Info("[Model::loadModel] Base directory: " + modelDir);
    
    processNode(scene->mRootNode, scene, glm::mat4(1.0f), modelDir);
}

void Model::Draw() {
    for (auto &submesh : submeshes) {
        if (submesh.VAO != 0)
            submesh.Draw();
    }
}