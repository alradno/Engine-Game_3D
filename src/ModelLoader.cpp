#include "core/ModelLoader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <string>
#include "utils/Logger.h"

void processNode(aiNode *node, const aiScene *scene,
                 std::vector<Vertex> &vertices,
                 std::vector<unsigned int> &indices,
                 const glm::mat4 &parentTransform)
{
    Logger::Debug("[ModelLoader] Processing node: " + std::string(node->mName.C_Str()));
    glm::mat4 nodeTransform = parentTransform;
    // Si se requiere aplicar la transformación del nodo, descomenta la siguiente línea:
    // nodeTransform = parentTransform * aiMatrix4x4ToGlm(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        size_t vertexOffset = vertices.size();
        Logger::Debug("[ModelLoader] Processing mesh " + std::to_string(i) +
                      " (" + std::to_string(mesh->mNumVertices) + " vertices)");

        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[j].x,
                                        mesh->mVertices[j].y,
                                        mesh->mVertices[j].z);
            vertex.Normal = mesh->HasNormals()
                                ? glm::normalize(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z))
                                : glm::vec3(0.0f);
            vertex.TexCoords = mesh->HasTextureCoords(0)
                                   ? glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y)
                                   : glm::vec2(0.0f);
            // Soporte para segundo set de UV
            if (mesh->HasTextureCoords(1))
                vertex.TexCoords2 = glm::vec2(mesh->mTextureCoords[1][j].x, mesh->mTextureCoords[1][j].y);
            else
                vertex.TexCoords2 = glm::vec2(0.0f);
            
            vertex.Tangent = mesh->HasTangentsAndBitangents()
                                 ? glm::normalize(glm::vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z))
                                 : glm::vec3(0.0f);
            vertices.push_back(vertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++)
            {
                indices.push_back(static_cast<unsigned int>(vertexOffset + face.mIndices[k]));
            }
        }
        Logger::Debug("[ModelLoader] Mesh " + std::to_string(i) +
                      " processed (" + std::to_string(mesh->mNumFaces) + " faces)");

        // Calcular tangentes manualmente si faltan
        if (!mesh->HasTangentsAndBitangents() && mesh->HasTextureCoords(0))
        {
            std::vector<glm::vec3> tempTangents(mesh->mNumVertices, glm::vec3(0.0f));
            for (unsigned int j = 0; j < mesh->mNumFaces; j++)
            {
                aiFace face = mesh->mFaces[j];
                if (face.mNumIndices < 3)
                    continue;
                unsigned int i0 = face.mIndices[0];
                unsigned int i1 = face.mIndices[1];
                unsigned int i2 = face.mIndices[2];
                glm::vec3 pos0 = vertices[vertexOffset + i0].Position;
                glm::vec3 pos1 = vertices[vertexOffset + i1].Position;
                glm::vec3 pos2 = vertices[vertexOffset + i2].Position;
                glm::vec2 uv0 = vertices[vertexOffset + i0].TexCoords;
                glm::vec2 uv1 = vertices[vertexOffset + i1].TexCoords;
                glm::vec2 uv2 = vertices[vertexOffset + i2].TexCoords;
                glm::vec3 edge1 = pos1 - pos0;
                glm::vec3 edge2 = pos2 - pos0;
                glm::vec2 deltaUV1 = uv1 - uv0;
                glm::vec2 deltaUV2 = uv2 - uv0;
                float det = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
                if (det == 0.0f)
                    Logger::Warning("[ModelLoader] Determinant is 0 for face " + std::to_string(j));
                float f = (det != 0.0f) ? 1.0f / det : 1.0f;
                glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
                tempTangents[i0] += tangent;
                tempTangents[i1] += tangent;
                tempTangents[i2] += tangent;
            }
            for (unsigned int j = 0; j < mesh->mNumVertices; j++)
            {
                vertices[vertexOffset + j].Tangent = glm::normalize(tempTangents[j]);
            }
            Logger::Debug("[ModelLoader] Manually calculated tangents.");
        }
    }
    Logger::Info("[ModelLoader] Finished processing node: " + std::string(node->mName.C_Str()));
}