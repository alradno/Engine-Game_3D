#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

/**
 * @struct TransformComponent
 * @brief Componente de transformación para una entidad.
 *
 * Se utiliza la convención glTF:
 *   - Sistema de coordenadas right-handed, con Y up.
 *   - La transformación final se construye como: T * R * S.
 *
 * Esto significa que, para transformar un vértice \(v\):
 *    v' = T * R * S * v
 * se aplica primero la escala, luego la rotación y por último la traslación.
 */
struct TransformComponent {
    glm::vec3 translation = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);  // (pitch, yaw, roll) en grados.
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 transform = glm::mat4(1.0f);

    // Actualiza la transformación final: T * R * S.
    void UpdateTransform() {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 R = glm::yawPitchRoll(glm::radians(rotation.y),
                                        glm::radians(rotation.x),
                                        glm::radians(rotation.z));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        transform = T * R * S;
    }
};
