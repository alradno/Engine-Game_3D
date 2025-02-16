#pragma once

#include <vector>
#include "Light.h"
#include "UniformBuffer.h"
#include "Logger.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class LightManager {
public:
    std::vector<Light> lights;
    UniformBuffer lightUBO;

    LightManager() {
        Logger::Info("[LightManager] Created and UBO initialized.");
    }
    
    void UpdateUBO() {
        Logger::Info("[LightManager] Updating UBO");
        constexpr int maxLights = 10;
        std::vector<Light> lightData(maxLights);
        for (int i = 0; i < maxLights; i++) {
            if (i < static_cast<int>(lights.size()))
                lightData[i] = lights[i];
            else {
                lightData[i].typeAndPadding = glm::vec4(-1, 0, 0, 0);
                lightData[i].position = glm::vec4(0.0f);
                lightData[i].direction = glm::vec4(0.0f);
                lightData[i].colorAndIntensity = glm::vec4(0.0f);
                lightData[i].spotParams = glm::vec4(0.0f);
            }
        }
        size_t dataSize = lightData.size() * sizeof(Light);
        lightUBO.Bind();
        glBufferData(GL_UNIFORM_BUFFER, dataSize, lightData.data(), GL_DYNAMIC_DRAW);
        lightUBO.Unbind();
        Logger::Info("[LightManager] UBO updated (" + std::to_string(lights.size()) +
                     " active lights, max " + std::to_string(maxLights) + ").");
    }
    
    void AddLight(const Light& light) {
        lights.push_back(light);
        Logger::Debug("[LightManager] Added light. Total: " + std::to_string(lights.size()));
    }
    
    void ClearLights() {
        lights.clear();
        Logger::Info("[LightManager] Cleared all lights.");
    }
};