#include "Config.h"
#include "Logger.h"
#include <filesystem>

Config Config::LoadFromFile(const std::string& configFilePath) {
    Config config;
    try {
        YAML::Node root = YAML::LoadFile(configFilePath);
        if (root["projectRoot"])
            config.projectRoot = root["projectRoot"].as<std::string>();
        if (root["assets"])
            config.assets = root["assets"].as<std::string>();
        if (root["shaders"])
            config.shaders = root["shaders"].as<std::string>();
        if (root["vertexShader"])
            config.vertexShader = root["vertexShader"].as<std::string>();
        if (root["defaultShader"])
            config.defaultShader = root["defaultShader"].as<std::string>();
        if (root["render"] && root["render"]["ambientColor"]) {
            auto ac = root["render"]["ambientColor"].as<std::vector<float>>();
            if (ac.size() >= 3)
                config.ambientColor = glm::vec3(ac[0], ac[1], ac[2]);
        }
        if (root["lights"]) {
            for (const auto& lightNode : root["lights"]) {
                LightConfig lc;
                if (lightNode["type"])
                    lc.type = lightNode["type"].as<std::string>();
                if (lightNode["position"]) {
                    auto pos = lightNode["position"].as<std::vector<float>>();
                    if (pos.size() >= 3)
                        lc.position = glm::vec3(pos[0], pos[1], pos[2]);
                }
                if (lightNode["color"]) {
                    auto col = lightNode["color"].as<std::vector<float>>();
                    if (col.size() >= 3)
                        lc.color = glm::vec3(col[0], col[1], col[2]);
                }
                config.lights.push_back(lc);
            }
        }
        Logger::Info("[Config] Loaded configuration from: " + configFilePath);
    } catch (const std::exception& e) {
        Logger::Error("[Config] Error loading config.yaml: " + std::string(e.what()));
    }
    return config;
}