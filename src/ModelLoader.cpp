#include "ModelLoader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <string>
#include "Logger.h"

// Converts an Assimp aiMatrix4x4 to a glm::mat4.
glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from) {
    Logger::Debug("[ModelLoader::aiMatrix4x4ToGlm] Converting aiMatrix4x4 to glm::mat4.");
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    Logger::Debug("[ModelLoader::aiMatrix4x4ToGlm] Conversion complete.");
    return to;
}

// Processes an Assimp node recursively. This function extracts vertices and indices from each mesh
// in the node, and optionally calculates tangents if they are missing.
void processNode(aiNode* node, const aiScene* scene,
                 std::vector<Vertex>& vertices,
                 std::vector<unsigned int>& indices,
                 const glm::mat4& parentTransform)
{
    Logger::Debug("[ModelLoader::processNode] Processing node: " + std::string(node->mName.C_Str()));
    
    // Start with the parent transformation.
    glm::mat4 nodeTransform = parentTransform;
    // Uncomment the following line to apply the node's transformation:
    // nodeTransform = parentTransform * aiMatrix4x4ToGlm(node->mTransformation);
    
    // Iterate through all meshes referenced by this node.
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        size_t vertexOffset = vertices.size();
        Logger::Debug("[ModelLoader::processNode] Processing mesh " + std::to_string(i) +
                      " with " + std::to_string(mesh->mNumVertices) + " vertices.");
        
        // Process each vertex of the mesh.
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
            vertices.push_back(vertex);
        }
        
        // Process the mesh faces (indices).
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                // Using size_t for vertexOffset, explicitly cast the sum to unsigned int.
                indices.push_back(static_cast<unsigned int>(vertexOffset + face.mIndices[k]));
            }
        }
        
        Logger::Debug("[ModelLoader::processNode] Mesh " + std::to_string(i) +
                      " has " + std::to_string(mesh->mNumFaces) + " faces.");
        
        // If tangents are missing and texture coordinates are available, calculate tangents manually.
        if (!mesh->HasTangentsAndBitangents() && mesh->HasTextureCoords(0)) {
            std::vector<glm::vec3> tempTangents(mesh->mNumVertices, glm::vec3(0.0f));
            for (unsigned int j = 0; j < mesh->mNumFaces; j++){
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
                if(det == 0.0f) {
                    Logger::Warning("[ModelLoader::processNode] Determinant is 0 for mesh face " + std::to_string(j));
                }
                float f = (det != 0.0f) ? 1.0f / det : 1.0f;
                glm::vec3 tangent;
                tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                tempTangents[i0] += tangent;
                tempTangents[i1] += tangent;
                tempTangents[i2] += tangent;
            }
            for (unsigned int j = 0; j < mesh->mNumVertices; j++){
                vertices[vertexOffset + j].Tangent = glm::normalize(tempTangents[j]);
            }
            Logger::Debug("[ModelLoader::processNode] Manually calculated tangents for mesh lacking them.");
        }
    }
    
    Logger::Info("[ModelLoader::processNode] Finished processing node: " + std::string(node->mName.C_Str()));
}