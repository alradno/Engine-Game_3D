#pragma once
#include "engine/Scene.h"
#include <memory>
#include "core/Coordinator.h"
#include "engine/SceneResources.h"
#include "systems/RenderSystem.h"
#include "engine/LightManager.h"
#include "renderer/Shader.h"
#include "engine/Camera.h"
#include "engine/ECSPlayerController.h"

class Scene1 : public Scene {
public:
    Scene1();
    virtual ~Scene1();
    
    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Destroy() override;
    
private:
    std::unique_ptr<Coordinator> coordinator;
    std::shared_ptr<Shader> shader;
    SceneResources sceneResources;
    std::shared_ptr<RenderSystem> renderSystem;
    std::unique_ptr<LightManager> lightManager;
    
    // Cámara propia para Scene1.
    Camera camera;
    
    // Delta time actual.
    float currentDeltaTime;
    
    // Controlador para mover el modelo (teclas W, A, S, D).
    std::unique_ptr<ECSPlayerController> playerController;
};