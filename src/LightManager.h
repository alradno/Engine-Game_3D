#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <vector>
#include "Light.h"
#include "UniformBuffer.h"
#include "Logger.h"

class LightManager {
public:
    std::vector<Light> lights;
    UniformBuffer lightUBO; // UBO para enviar la información de las luces

    LightManager() {
        // Se asume que el constructor de UniformBuffer genera el buffer.
    }
    
    // Actualiza el UBO para que siempre tenga 10 entradas.
    void UpdateUBO() {
        const int maxLights = 10;
        std::vector<Light> lightData(maxLights);
        for (int i = 0; i < maxLights; i++) {
            if (i < lights.size()) {
                lightData[i] = lights[i];
            } else {
                // Rellenar con una luz "vacía" (se indica con tipo -1)
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
        Logger::Info("[LightManager] UBO updated with " + std::to_string(lights.size()) + " lights.");
    }
    
    // Agrega una luz.
    void AddLight(const Light& light) {
        lights.push_back(light);
    }
    
    // Limpia la lista de luces.
    void ClearLights() {
        lights.clear();
    }
};

#endif // LIGHTMANAGER_H
