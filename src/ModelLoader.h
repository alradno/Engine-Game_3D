#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <glm/glm.hpp>
#include <vector>
#include <assimp/scene.h>

// Structure for storing vertex data (for PBR, tangent is required)
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

// Recursive function to process nodes and extract vertices and indices
void processNode(aiNode* node, const aiScene* scene,
                 std::vector<Vertex>& vertices,
                 std::vector<unsigned int>& indices,
                 const glm::mat4& parentTransform);

#endif
