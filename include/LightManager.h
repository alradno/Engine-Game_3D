#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <vector>
#include "Light.h"
#include "UniformBuffer.h"
#include "Logger.h"

class LightManager {
public:
    // Vector of lights managed by the LightManager.
    std::vector<Light> lights;
    // Uniform Buffer Object for sending light information to the GPU.
    UniformBuffer lightUBO;

    // Constructor: It is assumed that the UniformBuffer constructor creates the buffer.
    LightManager() {
        Logger::Info("[LightManager] LightManager instance created. UniformBuffer initialized.");
    }
    
    // Updates the UBO to always have 10 entries.
    void UpdateUBO() {
        Logger::Info("[LightManager] Updating UBO with current light data.");
        const int maxLights = 10;
        std::vector<Light> lightData(maxLights);
        for (int i = 0; i < maxLights; i++) {
            if (i < lights.size()) {
                lightData[i] = lights[i];
                Logger::Debug("[LightManager] Light at index " + std::to_string(i) + " updated from active lights.");
            } else {
                // Fill with an "empty" light (indicated by type -1).
                lightData[i].typeAndPadding = glm::vec4(-1, 0, 0, 0);
                lightData[i].position = glm::vec4(0.0f);
                lightData[i].direction = glm::vec4(0.0f);
                lightData[i].colorAndIntensity = glm::vec4(0.0f);
                lightData[i].spotParams = glm::vec4(0.0f);
                Logger::Debug("[LightManager] Light at index " + std::to_string(i) + " set to empty (no light).");
            }
        }
        size_t dataSize = lightData.size() * sizeof(Light);
        Logger::Debug("[LightManager] Binding UBO and uploading " + std::to_string(dataSize) + " bytes of light data.");
        lightUBO.Bind();
        glBufferData(GL_UNIFORM_BUFFER, dataSize, lightData.data(), GL_DYNAMIC_DRAW);
        lightUBO.Unbind();
        Logger::Info("[LightManager] UBO update complete. Active lights: " + std::to_string(lights.size()) +
                     " (max capacity: " + std::to_string(maxLights) + ").");
    }
    
    // Adds a light to the manager.
    void AddLight(const Light& light) {
        lights.push_back(light);
        Logger::Debug("[LightManager] New light added. Total active lights: " + std::to_string(lights.size()));
    }
    
    // Clears the list of lights.
    void ClearLights() {
        lights.clear();
        Logger::Info("[LightManager] All lights have been cleared.");
    }
};

#endif // LIGHTMANAGER_H