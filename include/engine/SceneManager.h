#pragma once
#include <memory>
#include "Scene.h"

/**
 * @brief SceneManager es un singleton que se encarga de mantener la escena actual
 * y cambiar entre escenas llamando a sus métodos Init() y Destroy().
 */
class SceneManager {
public:
    static SceneManager& GetInstance() {
        static SceneManager instance;
        return instance;
    }
    
    // Cambia la escena actual. Si había una anterior, se llama a Destroy() y se libera.
    void SwitchScene(std::unique_ptr<Scene> newScene) {
        if (currentScene) {
            currentScene->Destroy();
        }
        currentScene = std::move(newScene);
        if (currentScene) {
            currentScene->Init();
        }
    }
    
    void Update(float dt) {
        if (currentScene)
            currentScene->Update(dt);
    }
    
    void Render() {
        if (currentScene)
            currentScene->Render();
    }
    
private:
    std::unique_ptr<Scene> currentScene;
    
    // Constructor privado para forzar la condición de singleton
    SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};