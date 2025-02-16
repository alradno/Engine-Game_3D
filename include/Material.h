#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "Texture2D.h"

struct Material {
    std::shared_ptr<Texture2D> albedo;
    std::shared_ptr<Texture2D> metallicRoughness;
    std::shared_ptr<Texture2D> normal;

    Material() : albedo(nullptr), metallicRoughness(nullptr), normal(nullptr) { }
};

#endif
