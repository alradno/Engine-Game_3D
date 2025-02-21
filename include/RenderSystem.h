// RenderSystem.h
#pragma once

#include "System.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "Coordinator.h"
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

class RenderSystem : public System {
public:
    RenderSystem() : mCoordinator(nullptr), mShader(nullptr), mCamera(nullptr), mModelLoc(-1) { }
    
    void Init(Coordinator* coordinator, Shader* shader, Camera* camera);
    void Update(float dt);
    
private:
    Coordinator* mCoordinator;
    Shader* mShader;
    Camera* mCamera;
    int mModelLoc; // Ubicación cacheada de la uniform "model"
};