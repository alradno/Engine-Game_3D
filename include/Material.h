/**
 * @file Material.h
 * @brief Defines the Material structure that stores textures and material factors for PBR shading.
 */

#pragma once
#include <memory>
#include "Texture2D.h"
#include <glm/glm.hpp>

struct Material
{
    // Textures used by the material
    std::shared_ptr<Texture2D> albedo;
    std::shared_ptr<Texture2D> metallicRoughness;
    std::shared_ptr<Texture2D> normal;
    std::shared_ptr<Texture2D> occlusion;
    std::shared_ptr<Texture2D> emissive;

    // Material factors used for PBR shading
    glm::vec4 baseColorFactor = glm::vec4(1.0f); // RGBA; default is white
    float metallicFactor = 1.0f;                 // Default metallic factor
    float roughnessFactor = 1.0f;                // Default roughness factor
    glm::vec3 emissiveFactor = glm::vec3(0.0f);  // Default emissive factor (no emission)

    Material() = default;
};