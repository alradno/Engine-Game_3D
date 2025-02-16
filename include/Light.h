#pragma once

#include <glm/glm.hpp>
#include "Logger.h"
#include <string>

enum class LightType : int { POINT = 0, SPOT = 1, DIRECTIONAL = 2 };

struct Light {
    glm::vec4 typeAndPadding;   // x: tipo, yzw: padding
    glm::vec4 position;         // xyz: posición, w: padding
    glm::vec4 direction;        // xyz: dirección, w: padding
    glm::vec4 colorAndIntensity;// rgb: color, a: intensidad
    glm::vec4 spotParams;       // x: cutOff, y: outerCutOff, z/w: padding

    inline void LogLightDetails() const {
        Logger::Debug("[Light] Type: " + std::to_string(typeAndPadding.x));
        Logger::Debug("[Light] Position: (" + std::to_string(position.x) + ", " +
                      std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
        Logger::Debug("[Light] Direction: (" + std::to_string(direction.x) + ", " +
                      std::to_string(direction.y) + ", " + std::to_string(direction.z) + ")");
        Logger::Debug("[Light] Color: (" + std::to_string(colorAndIntensity.x) + ", " +
                      std::to_string(colorAndIntensity.y) + ", " + std::to_string(colorAndIntensity.z) + ")");
        Logger::Debug("[Light] Intensity: " + std::to_string(colorAndIntensity.w));
        Logger::Debug("[Light] Spot Params: (cutOff: " + std::to_string(spotParams.x) +
                      ", outerCutOff: " + std::to_string(spotParams.y) + ")");
    }
};