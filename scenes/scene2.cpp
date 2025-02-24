#include "Scene2.h"
#include "core/EntityLoader.h"
#include "systems/RenderSystem.h"
#include "renderer/ResourceManager.h"
#include "utils/Logger.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

Scene2::Scene2() : renderSystem(nullptr), currentDeltaTime(0.0f) { }

Scene2::~Scene2() {
    Destroy();
}

void Scene2::Init() {
    Logger::Info("[Scene2] Inicializando escena 2");
    
    coordinator = std::make_unique<Coordinator>();
    coordinator->Init();
    coordinator->RegisterComponent<TransformComponent>();
    coordinator->RegisterComponent<RenderComponent>();
    
    renderSystem = coordinator->RegisterSystem<RenderSystem>();
    ECS::Signature signature;
    signature.set(coordinator->GetComponentType<TransformComponent>());
    signature.set(coordinator->GetComponentType<RenderComponent>());
    coordinator->SetSystemSignature<RenderSystem>(signature);
    
    shader = sceneResources.LoadShader("pbr_vertex.glsl", "pbr_fragment.glsl", "scene2Shader");
    if (!shader) {
        Logger::Error("[Scene2] Error al cargar shader 'scene2Shader'.");
        return;
    }
    
    // Configurar la cámara de Scene2.
    camera.Position = glm::vec3(0.0f, 10.0f, 20.0f);
    camera.Front = glm::normalize(glm::vec3(0.0f) - camera.Position);
    camera.Up = glm::vec3(0, 1, 0);
    
    // Inicializar el RenderSystem con el coordinator, el shader y la cámara.
    renderSystem->Init(coordinator.get(), shader.get(), &camera);
    
    const Config& config = ResourceManager::GetConfig();
    lightManager = std::make_unique<LightManager>();
    for (const auto &lc : config.lights) {
        Light light{};
        if (lc.type == "point")
            light.typeAndPadding = glm::vec4(0, 0, 0, 0);
        light.position = glm::vec4(lc.position, 1.0f);
        light.colorAndIntensity = glm::vec4(lc.color, 1.0f);
        lightManager->AddLight(light);
    }
    unsigned int lightBlockIndex = glGetUniformBlockIndex(shader->ID, "LightBlock");
    if (lightBlockIndex == GL_INVALID_INDEX) {
        Logger::Error("[Scene2] 'LightBlock' uniform block not found in shader.");
    } else {
        GLCall(glUniformBlockBinding(shader->ID, lightBlockIndex, 1));
        Logger::Info("[Scene2] LightBlock bound to binding point 1.");
    }
    
    EntityLoader::LoadEntitiesFromYAML(coordinator.get(), "./config/entities_scene2.yaml");
    
    // Asumir que la primera entidad (ID 0) es el vehículo del jugador; crear el controlador.
    playerController = std::make_unique<ECSPlayerController>(coordinator.get(), 0);
    Logger::Info("[Scene2] Escena 2 inicializada.");
}

void Scene2::Update(float dt) {
    currentDeltaTime = dt;
    
    if (playerController) {
        playerController->Update(dt);
    }
    
    if (renderSystem) {
        renderSystem->Update(dt);
    }
}

void Scene2::Render() {
    if (!shader) return;
    shader->Use();

    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "albedoMap"), 0));
    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "metallicRoughnessMap"), 1));
    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "normalMap"), 2));
    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "useMaps"), 1));
    
    glm::mat4 view = camera.GetViewMatrix();
    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view)));
    GLCall(glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1, glm::value_ptr(camera.Position)));
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 100.0f);
    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
    
    if (lightManager) {
        lightManager->UpdateUBO();
        lightManager->lightUBO.BindToPoint(1);
        GLCall(glUniform3fv(glGetUniformLocation(shader->ID, "ambientColor"), 1, glm::value_ptr(ResourceManager::GetConfig().ambientColor)));
    }
    
    if (renderSystem) {
        renderSystem->Update(currentDeltaTime);
    }
}

void Scene2::Destroy() {
    Logger::Info("[Scene2] Destruyendo escena 2");
    if (coordinator) {
        coordinator->Clear();
        coordinator.reset();
    }
    sceneResources.Clear();
    if (lightManager) {
        lightManager->ClearLights();
        lightManager.reset();
    }
    // (Opcional) Llamar a ResourceManager::Clear() para limpiar recursos globales.
    Logger::Info("[Scene2] Escena 2 destruida.");
}