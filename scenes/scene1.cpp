#include "Scene1.h"
#include "core/EntityLoader.h"
#include "systems/RenderSystem.h"
#include "renderer/ResourceManager.h"
#include "utils/Logger.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

Scene1::Scene1() : renderSystem(nullptr), currentDeltaTime(0.0f) { }

Scene1::~Scene1() {
    Destroy();
}

void Scene1::Init() {
    Logger::Info("[Scene1] Inicializando escena 1");
    
    // Inicializamos el coordinator exclusivo para Scene1.
    coordinator = std::make_unique<Coordinator>();
    coordinator->Init();
    coordinator->RegisterComponent<TransformComponent>();
    coordinator->RegisterComponent<RenderComponent>();
    
    renderSystem = coordinator->RegisterSystem<RenderSystem>();
    ECS::Signature signature;
    signature.set(coordinator->GetComponentType<TransformComponent>());
    signature.set(coordinator->GetComponentType<RenderComponent>());
    coordinator->SetSystemSignature<RenderSystem>(signature);
    
    // Cargar el shader exclusivo para Scene1 (se reutiliza si ya fue cargado).
    shader = sceneResources.LoadShader("pbr_vertex.glsl", "pbr_fragment.glsl", "scene1Shader");
    if (!shader) {
        Logger::Error("[Scene1] Error al cargar shader 'scene1Shader'.");
        return;
    }
    
    // Configurar la cámara propia.
    camera.Position = glm::vec3(0.0f, 10.0f, 20.0f);
    camera.Front = glm::normalize(glm::vec3(0.0f) - camera.Position);
    camera.Up = glm::vec3(0, 1, 0);
    
    // Inicializar el RenderSystem con el coordinator, el shader y la cámara.
    renderSystem->Init(coordinator.get(), shader.get(), &camera);
    
    // Configurar las luces usando la configuración global.
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
        Logger::Error("[Scene1] 'LightBlock' uniform block not found in shader.");
    } else {
        GLCall(glUniformBlockBinding(shader->ID, lightBlockIndex, 1));
        Logger::Info("[Scene1] LightBlock bound to binding point 1.");
    }
    
    // Cargar las entidades específicas de Scene1.
    EntityLoader::LoadEntitiesFromYAML(coordinator.get(), "./config/entities_scene1.yaml");
    
    // Asumir que la primera entidad (ID 0) es el vehículo del jugador; crear el controlador.
    playerController = std::make_unique<ECSPlayerController>(coordinator.get(), 0);
    
    Logger::Info("[Scene1] Escena 1 inicializada.");
}

void Scene1::Update(float dt) {
    currentDeltaTime = dt;
    
    // Actualizar el controlador para mover el modelo (teclas W, A, S, D).
    if (playerController) {
        playerController->Update(dt);
    }
    
    if (renderSystem) {
        renderSystem->Update(dt);
    }
}

void Scene1::Render() {
    if (!shader) return;
    shader->Use();

    // Configurar los samplers.
    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "albedoMap"), 0));
    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "metallicRoughnessMap"), 1));
    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "normalMap"), 2));
    GLCall(glUniform1i(glGetUniformLocation(shader->ID, "useMaps"), 1));
    
    // Configurar los uniforms de la cámara.
    glm::mat4 view = camera.GetViewMatrix();
    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view)));
    GLCall(glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1, glm::value_ptr(camera.Position)));
    
    // Configurar la proyección.
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 100.0f);
    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
    
    // Actualizar y vincular el UBO de las luces.
    if (lightManager) {
        lightManager->UpdateUBO();
        lightManager->lightUBO.BindToPoint(1);
        const Config& config = ResourceManager::GetConfig();
        GLCall(glUniform3fv(glGetUniformLocation(shader->ID, "ambientColor"), 1, glm::value_ptr(config.ambientColor)));
    }
    
    // Llamar al RenderSystem para renderizar las entidades.
    if (renderSystem) {
        renderSystem->Update(currentDeltaTime);
    }
}

void Scene1::Destroy() {
    Logger::Info("[Scene1] Destruyendo escena 1");
    // Limpiar todas las entidades del ECS.
    if (coordinator) {
        coordinator->Clear();
        coordinator.reset();
    }
    // Liberar recursos exclusivos de escena.
    sceneResources.Clear();
    if (lightManager) {
        lightManager->ClearLights();
        lightManager.reset();
    }
    // (Opcional) Limpiar recursos globales si se desea:
    // ResourceManager::Clear();
    Logger::Info("[Scene1] Escena 1 destruida.");
}