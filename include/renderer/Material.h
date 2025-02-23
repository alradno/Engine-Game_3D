#pragma once
#include <memory>
#include "Texture2D.h"
#include <glm/glm.hpp>

struct Material
{
    // Texturas utilizadas en el material
    std::shared_ptr<Texture2D> albedo;
    std::shared_ptr<Texture2D> metallicRoughness;
    std::shared_ptr<Texture2D> normal;
    std::shared_ptr<Texture2D> occlusion;
    std::shared_ptr<Texture2D> emissive;

    // Factores base para PBR
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    glm::vec3 emissiveFactor = glm::vec3(0.0f);

    // Nuevas propiedades para clearcoat (KHR_materials_clearcoat)
    float clearcoatFactor = 0.0f;           // Por defecto 0: sin clearcoat.
    float clearcoatRoughnessFactor = 0.0f;    // Por defecto 0: idealmente suave.

    // Propiedades para transmission (KHR_materials_transmission)
    float transmissionFactor = 0.0f;          // Por defecto 0: no transmite luz.
    float ior = 1.45f;                        // Índice de refracción (valor típico para vidrio)
};