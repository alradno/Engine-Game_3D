#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "Logger.h"   // Include Logger for detailed logging output
#include <string>

// Define light types using an integer:
// 0: Point light
// 1: Spot light
// 2: Directional light (optional)
enum class LightType : int {
    POINT = 0,
    SPOT = 1,
    DIRECTIONAL = 2
};

// Structure to store the information of a light in std140 layout,
// using vec4 for proper alignment (80 bytes per light).
struct Light {
    // Stores the light type in the x component; y, z, w are used as padding.
    glm::vec4 typeAndPadding;
    // Position of the light: xyz represents the position and w is used as padding.
    glm::vec4 position;
    // Direction of the light: xyz represents the direction and w is used as padding.
    glm::vec4 direction;
    // Color and intensity: rgb represents the color, and a represents the intensity.
    glm::vec4 colorAndIntensity;
    // Parameters for spot lights: x = cutOff, y = outerCutOff, z and w are used as padding.
    glm::vec4 spotParams;

    // Logs detailed information about the light's properties.
    inline void LogLightDetails() const {
        Logger::Debug("[Light] Logging light details:");
        Logger::Debug("  Type (stored in x component): " + std::to_string(typeAndPadding.x));
        Logger::Debug("  Position: (" +
                      std::to_string(position.x) + ", " +
                      std::to_string(position.y) + ", " +
                      std::to_string(position.z) + ")");
        Logger::Debug("  Direction: (" +
                      std::to_string(direction.x) + ", " +
                      std::to_string(direction.y) + ", " +
                      std::to_string(direction.z) + ")");
        Logger::Debug("  Color (RGB): (" +
                      std::to_string(colorAndIntensity.x) + ", " +
                      std::to_string(colorAndIntensity.y) + ", " +
                      std::to_string(colorAndIntensity.z) + ")");
        Logger::Debug("  Intensity (stored in w component): " + std::to_string(colorAndIntensity.w));
        Logger::Debug("  Spot Params: (cutOff: " +
                      std::to_string(spotParams.x) + ", outerCutOff: " +
                      std::to_string(spotParams.y) + ")");
    }
};

#endif // LIGHT_H