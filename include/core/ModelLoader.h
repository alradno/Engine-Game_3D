#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <assimp/scene.h>
#include "utils/Logger.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec2 TexCoords2; // Segundo set de coordenadas UV
    glm::vec3 Tangent;
};

void processNode(aiNode* node, const aiScene* scene,
                 std::vector<Vertex>& vertices,
                 std::vector<unsigned int>& indices,
                 const glm::mat4& parentTransform);
