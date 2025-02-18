#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

// Estructura para la configuración de luces
struct LightConfig {
    std::string type;
    glm::vec3 position;
    glm::vec3 color;
};

class Config {
public:
    // Parámetros globales
    std::string projectRoot;
    std::string assets;        // Directorio base para assets
    std::string shaders;       // Directorio de shaders
    std::string vertexShader;  // Nombre del vertex shader global (sin extensión)
    std::string defaultShader; // Nombre del fragment shader por defecto (sin extensión)
    glm::vec3 ambientColor;
    std::vector<LightConfig> lights;

    static Config LoadFromFile(const std::string& configFilePath);
};