/**
 * @file EntityLoader.cpp
 * @brief Implementation of the EntityLoader class that loads entities from a YAML file.
 */

#include "core/EntityLoader.h"
#include <yaml-cpp/yaml.h>
#include "utils/Logger.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "components/TextComponent.h"   // Incluir para cargar el texto
#include "components/ButtonComponent.h" // Incluir para cargar el botón
#include "renderer/ResourceManager.h"
#include "renderer/Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

void EntityLoader::LoadEntitiesFromYAML(Coordinator *coordinator, const std::string &filename)
{
    YAML::Node config;
    try
    {
        config = YAML::LoadFile(filename);
    }
    catch (const YAML::Exception &e)
    {
        Logger::Error("[EntityLoader] Failed to load YAML file: " + filename + " Error: " + e.what());
        return;
    }

    if (!config["entities"])
    {
        Logger::Error("[EntityLoader] No 'entities' node found in " + filename);
        return;
    }

    for (const auto &entityNode : config["entities"])
    {
        ECS::Entity entity = coordinator->CreateEntity();

        // Load TransformComponent if present
        if (entityNode["transform"])
        {
            TransformComponent transform;
            if (entityNode["transform"]["translation"])
            {
                std::vector<float> t = entityNode["transform"]["translation"].as<std::vector<float>>();
                if (t.size() >= 3)
                {
                    transform.translation = glm::vec3(t[0], t[1], t[2]);
                    Logger::Debug("[EntityLoader] Translation loaded: " +
                                  std::to_string(t[0]) + ", " +
                                  std::to_string(t[1]) + ", " +
                                  std::to_string(t[2]));
                }
            }
            if (entityNode["transform"]["rotation"])
            {
                std::vector<float> r = entityNode["transform"]["rotation"].as<std::vector<float>>();
                if (r.size() >= 3)
                {
                    transform.rotation = glm::vec3(r[0], r[1], r[2]);
                    Logger::Debug("[EntityLoader] Rotation loaded: " +
                                  std::to_string(r[0]) + ", " +
                                  std::to_string(r[1]) + ", " +
                                  std::to_string(r[2]));
                }
            }
            if (entityNode["transform"]["scale"])
            {
                std::vector<float> s = entityNode["transform"]["scale"].as<std::vector<float>>();
                if (s.size() >= 3)
                {
                    transform.scale = glm::vec3(s[0], s[1], s[2]);
                    Logger::Debug("[EntityLoader] Scale loaded: " +
                                  std::to_string(s[0]) + ", " +
                                  std::to_string(s[1]) + ", " +
                                  std::to_string(s[2]));
                }
            }
            transform.UpdateTransform();
            coordinator->AddComponent<TransformComponent>(entity, transform);
        }

        // Load RenderComponent if present
        if (entityNode["render"])
        {
            RenderComponent render;
            if (entityNode["render"]["model"])
            {
                std::string modelPath = entityNode["render"]["model"].as<std::string>();
                render.model = ResourceManager::LoadModel(modelPath.c_str(), modelPath);
            }
            coordinator->AddComponent<RenderComponent>(entity, render);
        }

        // Load TextComponent if present
        if (entityNode["text"])
        {
            TextComponent textComp;
            if (entityNode["text"]["content"])
            {
                textComp.content = entityNode["text"]["content"].as<std::string>();
            }
            if (entityNode["text"]["font"])
            {
                textComp.font = entityNode["text"]["font"].as<std::string>();
            }
            if (entityNode["text"]["size"])
            {
                textComp.size = entityNode["text"]["size"].as<int>();
            }
            if (entityNode["text"]["color"])
            {
                std::vector<float> col = entityNode["text"]["color"].as<std::vector<float>>();
                if (col.size() >= 4)
                {
                    textComp.color = glm::vec4(col[0], col[1], col[2], col[3]);
                }
            }
            Logger::Debug("[EntityLoader] TextComponent loaded with content: " + textComp.content);
            coordinator->AddComponent<TextComponent>(entity, textComp);
        }

        // Load ButtonComponent if present
        if (entityNode["button"])
        {
            ButtonComponent btnComp;
            if (entityNode["button"]["action"])
            {
                btnComp.action = entityNode["button"]["action"].as<std::string>();
            }
            Logger::Debug("[EntityLoader] ButtonComponent loaded with action: " + btnComp.action);
            coordinator->AddComponent<ButtonComponent>(entity, btnComp);
        }

        Logger::Info("[EntityLoader] Created entity: " + std::to_string(entity));
    }
}