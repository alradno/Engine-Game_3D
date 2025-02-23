#pragma once

#include <string>
#include <vector>
#include "renderer/Submesh.h"
#include "utils/Logger.h"
#include <assimp/scene.h>
#include <glm/glm.hpp>

// Función inline para convertir un aiMatrix4x4 a glm::mat4
inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

class Model {
public:
    // Constructor: carga el modelo desde el archivo especificado
    Model(const std::string &path);

    // Método para dibujar el modelo
    void Draw();

    // Vector de submeshes
    std::vector<Submesh> submeshes;

private:
    // Método para cargar el modelo
    void loadModel(const std::string &path);

    // Función recursiva para procesar la jerarquía de nodos
    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform, const std::string &modelDir);
};