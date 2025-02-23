// RenderSystem.cpp
#include "systems/RenderSystem.h"
#include "core/Coordinator.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "renderer/Shader.h"
#include "engine/Camera.h"
#include "utils/GLDebug.h"
#include "utils/Logger.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

void RenderSystem::Init(Coordinator* coordinator, Shader* shader, Camera* camera) {
    mCoordinator = coordinator;
    mShader = shader;
    mCamera = camera;
    // Cachear la ubicación de la uniform "model"
    mModelLoc = glGetUniformLocation(mShader->ID, "model");
}

void RenderSystem::Update(float dt) {
    if (!mCoordinator || !mShader || !mCamera) return;

    glm::vec3 camPos = mCamera->Position;

    // Recoger entidades y ordenarlas por el puntero al Model para agrupar draw calls
    std::vector<std::pair<Model*, ECS::Entity>> sortedEntities;
    for (auto entity : mEntities) {
        auto& render = mCoordinator->GetComponent<RenderComponent>(entity);
        if (render.model) {
            sortedEntities.push_back({ render.model.get(), entity });
        }
    }
    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const std::pair<Model*, ECS::Entity>& a, const std::pair<Model*, ECS::Entity>& b) {
            return a.first < b.first;
        }
    );

    // Renderizar entidades ordenadas con culling simple (omitiendo aquellas muy lejanas)
    for (const auto& pair : sortedEntities) {
        ECS::Entity entity = pair.second;
        auto& transform = mCoordinator->GetComponent<TransformComponent>(entity);
        if (glm::distance(transform.translation, camPos) > 100.0f)
            continue;
        transform.UpdateTransform();
        GLCall(glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, glm::value_ptr(transform.transform)));
        auto& render = mCoordinator->GetComponent<RenderComponent>(entity);
        if (render.model)
            render.model->Draw();
    }
}