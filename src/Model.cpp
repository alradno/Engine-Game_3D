/**
 * @file Model.cpp
 * @brief Implementation of the Model class. Loads a 3D model from a glTF file using Assimp,
 * processes meshes and materials—including additional textures (occlusion and emissive)
 * and material factors (base color, metallic, roughness, emissive).
 */

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
#include <glm/gtc/type_ptr.hpp>

// Helper function: returns true if the given path is non-empty and has a file extension.
bool isValidTexturePath(const std::string &path)
{
    if (path.empty())
        return false;
    std::filesystem::path p(path);
    return p.has_extension();
}

void Model::loadModel(const std::string &path)
{
    Logger::Info("[Model::loadModel] Starting load: " + path);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Logger::Error("[Model::loadModel] Failed to load file: " + path +
                      "\nReason: " + importer.GetErrorString());
        return;
    }

    std::filesystem::path modelFilePath(path);
    std::string modelDir = modelFilePath.parent_path().generic_string();
    Logger::Info("[Model::loadModel] Base directory: " + modelDir);

    // Process each mesh in the scene
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];
        Submesh submesh;
        Logger::Info("[Model::loadModel] Processing mesh " + std::to_string(i) +
                     " with " + std::to_string(mesh->mNumVertices) + " vertices.");

        // Process vertices
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[j].x,
                                        mesh->mVertices[j].y,
                                        mesh->mVertices[j].z);
            vertex.Normal = mesh->HasNormals()
                                ? glm::normalize(glm::vec3(mesh->mNormals[j].x,
                                                           mesh->mNormals[j].y,
                                                           mesh->mNormals[j].z))
                                : glm::vec3(0.0f);
            vertex.TexCoords = mesh->HasTextureCoords(0)
                                   ? glm::vec2(mesh->mTextureCoords[0][j].x,
                                               mesh->mTextureCoords[0][j].y)
                                   : glm::vec2(0.0f);
            vertex.Tangent = mesh->HasTangentsAndBitangents()
                                 ? glm::normalize(glm::vec3(mesh->mTangents[j].x,
                                                            mesh->mTangents[j].y,
                                                            mesh->mTangents[j].z))
                                 : glm::vec3(0.0f);
            submesh.vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++)
                submesh.indices.push_back(face.mIndices[k]);
        }
        Logger::Info("[Model::loadModel] Mesh " + std::to_string(i) +
                     " has " + std::to_string(mesh->mNumFaces) + " faces.");

        // Process materials and textures
        if (scene->HasMaterials())
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            // ALBEDO TEXTURE (try BASE_COLOR then DIFFUSE)
            std::string albedoPath = GetTexturePath(material, aiTextureType_BASE_COLOR, modelDir);
            if (albedoPath.empty())
                albedoPath = GetTexturePath(material, aiTextureType_DIFFUSE, modelDir);
            if (isValidTexturePath(albedoPath))
            {
                Logger::Info("[Model::loadModel] Loading albedo texture: " + albedoPath);
                submesh.material.albedo = ResourceManager::LoadTexture(albedoPath.c_str(), true, albedoPath);
            }
            else
            {
                Logger::Debug("[Model::loadModel] No valid albedo texture specified for mesh " + std::to_string(i));
            }

            // NORMAL MAP
            std::string normalPath = GetTexturePath(material, aiTextureType_NORMALS, modelDir);
            if (isValidTexturePath(normalPath))
            {
                Logger::Info("[Model::loadModel] Loading normal texture: " + normalPath);
                submesh.material.normal = ResourceManager::LoadTexture(normalPath.c_str(), false, normalPath);
            }
            else
            {
                Logger::Debug("[Model::loadModel] No valid normal texture specified for mesh " + std::to_string(i));
            }

            // METALLIC/ROUGHNESS TEXTURE (try UNKNOWN then SPECULAR)
            std::string mrPath = GetTexturePath(material, aiTextureType_UNKNOWN, modelDir);
            if (mrPath.empty())
                mrPath = GetTexturePath(material, aiTextureType_SPECULAR, modelDir);
            if (isValidTexturePath(mrPath))
            {
                Logger::Info("[Model::loadModel] Loading metallic/roughness texture: " + mrPath);
                submesh.material.metallicRoughness = ResourceManager::LoadTexture(mrPath.c_str(), false, mrPath);
            }
            else
            {
                Logger::Debug("[Model::loadModel] No valid metallic/roughness texture specified for mesh " + std::to_string(i));
            }

            // OCCLUSION TEXTURE (try LIGHTMAP then AMBIENT)
            std::string occlusionPath = GetTexturePath(material, aiTextureType_LIGHTMAP, modelDir);
            if (occlusionPath.empty())
                occlusionPath = GetTexturePath(material, aiTextureType_AMBIENT, modelDir);
            if (isValidTexturePath(occlusionPath))
            {
                Logger::Info("[Model::loadModel] Loading occlusion texture: " + occlusionPath);
                submesh.material.occlusion = ResourceManager::LoadTexture(occlusionPath.c_str(), false, occlusionPath);
            }
            else
            {
                Logger::Debug("[Model::loadModel] No valid occlusion texture specified for mesh " + std::to_string(i));
            }

            // EMISSIVE TEXTURE
            std::string emissivePath = GetTexturePath(material, aiTextureType_EMISSIVE, modelDir);
            if (isValidTexturePath(emissivePath))
            {
                Logger::Info("[Model::loadModel] Loading emissive texture: " + emissivePath);
                submesh.material.emissive = ResourceManager::LoadTexture(emissivePath.c_str(), false, emissivePath);
            }
            else
            {
                Logger::Debug("[Model::loadModel] No valid emissive texture specified for mesh " + std::to_string(i));
            }

            // Extract material factors
            // Base Color Factor from diffuse color
            aiColor4D diffuseColor;
            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
            {
                submesh.material.baseColorFactor = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
                Logger::Debug("[Model::loadModel] Base color factor: " +
                              std::to_string(diffuseColor.r) + ", " +
                              std::to_string(diffuseColor.g) + ", " +
                              std::to_string(diffuseColor.b) + ", " +
                              std::to_string(diffuseColor.a));
            }

            // Metallic and Roughness factors
            float metallic = 1.0f, roughness = 1.0f;
            if (AI_SUCCESS == material->Get<float>("pbrMetallicRoughness.metallicFactor", 0, 0, metallic))
            {
                submesh.material.metallicFactor = metallic;
                Logger::Debug("[Model::loadModel] Metallic factor: " + std::to_string(metallic));
            }
            if (AI_SUCCESS == material->Get<float>("pbrMetallicRoughness.roughnessFactor", 0, 0, roughness))
            {
                submesh.material.roughnessFactor = roughness;
                Logger::Debug("[Model::loadModel] Roughness factor: " + std::to_string(roughness));
            }

            // Emissive factor
            aiColor4D emissiveColor;
            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emissiveColor))
            {
                submesh.material.emissiveFactor = glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
                Logger::Debug("[Model::loadModel] Emissive factor: " +
                              std::to_string(emissiveColor.r) + ", " +
                              std::to_string(emissiveColor.g) + ", " +
                              std::to_string(emissiveColor.b));
            }
        }

        submesh.setupMesh();
        Logger::Info("[Model::loadModel] Mesh " + std::to_string(i) + " loaded: " +
                     std::to_string(submesh.vertices.size()) + " vertices, " +
                     std::to_string(submesh.indices.size()) + " indices.");
        submeshes.push_back(submesh);
    }
    Logger::Info("[Model::loadModel] Total submeshes loaded: " + std::to_string(submeshes.size()));
}

void Model::Draw()
{
    Logger::Info("[Model::Draw] Drawing model with " + std::to_string(submeshes.size()) + " submeshes.");
    for (size_t i = 0; i < submeshes.size(); ++i)
    {
        if (submeshes[i].vertices.empty() || submeshes[i].indices.empty())
        {
            Logger::Warning("[Model::Draw] Submesh " + std::to_string(i) + " is invalid. Skipping draw.");
            continue;
        }
        submeshes[i].Draw();
    }
}