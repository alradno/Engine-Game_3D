#ifndef SCENE_H
#define SCENE_H

#include "SceneNode.h"
#include "Shader.h"
#include "Logger.h"

/// Clase que representa la escena completa.
class Scene {
public:
    Scene() {
        root = std::make_shared<SceneNode>();
        Logger::Info("[Scene] Scene created with an empty root node.");
    }
    
    void Update() {
        root->Update();
        Logger::Debug("[Scene] Scene updated.");
    }
    
    void Render(const Shader &shader) {
        root->Render(shader);
        Logger::Debug("[Scene] Scene rendered.");
    }
    
    std::shared_ptr<SceneNode> GetRoot() {
        return root;
    }
    
private:
    std::shared_ptr<SceneNode> root;
};

#endif // SCENE_H
