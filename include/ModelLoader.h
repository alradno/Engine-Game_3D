#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <glm/glm.hpp>
#include <vector>
#include <assimp/scene.h>
#include "Logger.h"

// Structure for storing vertex data (required for PBR shading; tangent is essential for normal mapping).
struct Vertex {
    glm::vec3 Position;   // The vertex position in 3D space.
    glm::vec3 Normal;     // The normal vector used for lighting calculations.
    glm::vec2 TexCoords;  // Texture coordinates for mapping textures.
    glm::vec3 Tangent;    // Tangent vector for correct normal mapping.
};

// Recursive function to process nodes in an Assimp scene graph.
// This function extracts vertices and indices from each mesh and applies the parent transform.
// Detailed debug logs are emitted during processing using Logger.
void processNode(aiNode* node, const aiScene* scene,
                 std::vector<Vertex>& vertices,
                 std::vector<unsigned int>& indices,
                 const glm::mat4& parentTransform);

#endif // MODELLOADER_H
