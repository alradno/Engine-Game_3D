#pragma once
#include "engine/Scene.h"
#include <memory>
#include "core/Coordinator.h"
#include "engine/SceneResources.h"
#include "systems/RenderSystem.h"
#include "renderer/Shader.h"

// Componentes de UI.
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"

// Incluir el TextRenderer.
#include "renderer/TextRenderer.h"

class MenuScene : public Scene {
public:
    MenuScene();
    virtual ~MenuScene();

    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Destroy() override;

private:
    std::unique_ptr<Coordinator> coordinator;
    std::shared_ptr<Shader> shader;
    SceneResources sceneResources;
    std::shared_ptr<RenderSystem> renderSystem;

    // Variables para renderizado de UI (el quad para el botón)
    unsigned int uiVAO, uiVBO, uiEBO;
    unsigned int whiteTexture;

    // Renderizador de texto.
    TextRenderer* textRenderer;
};