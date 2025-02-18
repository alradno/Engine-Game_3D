#include "Model.h"
#include "Submesh.h"
#include "Material.h"
#include "ResourceManager.h" // Si necesitas acceder a recursos para materiales
#include "FileUtils.h"
#include "Logger.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <sstream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

// Función para cargar un material a partir de un aiMaterial y la ruta base
Material LoadMaterial(aiMaterial* material, const std::string &modelDir) {
    Material mat;
    aiString texPath;
    if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == AI_SUCCESS) {
        std::string texPathStr = texPath.C_Str();
        if (!texPathStr.empty() && texPathStr.front() == '/')
            texPathStr.erase(0, 1);
        std::string fullTexPath = FileUtils::ResolvePath(modelDir, texPathStr);
        Logger::Debug("[LoadMaterial] Loading texture from: " + fullTexPath);
        mat.albedo = ResourceManager::LoadTexture(fullTexPath.c_str(), true, fullTexPath);
    } else {
        mat.baseColorFactor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    return mat;
}

// Constructor: llama a loadModel
Model::Model(const std::string &path) {
    Logger::Info("[Model] Loading from: " + path);
    loadModel(path);
}

// Recorre la jerarquía de nodos y procesa cada malla
void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform, const std::string &modelDir) {
    glm::mat4 nodeTransform = parentTransform * aiMatrix4x4ToGlm(node->mTransformation);
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Submesh submesh;
        Logger::Info("[Model::processNode] Processing mesh from node: " + std::string(node->mName.C_Str()) +
                     ", vertices: " + std::to_string(mesh->mNumVertices));
        
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
            
            if (mesh->HasTextureCoords(0))
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][j].x,
                                             mesh->mTextureCoords[0][j].y);
            else
                vertex.TexCoords = glm::vec2(0.0f);
            
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

// Carga el modelo usando Assimp y procesa la jerarquía de nodos.
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

// Dibuja cada submesh
void Model::Draw() {
    for (auto &submesh : submeshes) {
        if (submesh.VAO != 0)
            submesh.Draw();
    }
}