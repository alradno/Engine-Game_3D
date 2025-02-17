#pragma once

#include "System.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "Coordinator.h"
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * @class RenderSystem
 * @brief Sistema de renderizado para entidades que tienen TransformComponent y RenderComponent.
 */
class RenderSystem : public System {
public:
    RenderSystem() : mCoordinator(nullptr), mShader(nullptr), mCamera(nullptr) { }
    
    void Init(Coordinator* coordinator, Shader* shader, Camera* camera) {
        mCoordinator = coordinator;
        mShader = shader;
        mCamera = camera;
    }
    
    void Update(float dt) {
        if (!mCoordinator || !mShader || !mCamera) return;
        
        // Para cada entidad que tenga TransformComponent y RenderComponent:
        for (auto entity : mEntities) {
            auto& transform = mCoordinator->GetComponent<TransformComponent>(entity);
            // Actualiza la transformación usando el valor almacenado en rotation.y
            transform.UpdateTransform();
            glUniformMatrix4fv(glGetUniformLocation(mShader->ID, "model"), 1, GL_FALSE, &transform.transform[0][0]);
            auto& render = mCoordinator->GetComponent<RenderComponent>(entity);
            if (render.model)
                render.model->Draw();
        }
    }
    
private:
    Coordinator* mCoordinator;
    Shader* mShader;
    Camera* mCamera;
};
