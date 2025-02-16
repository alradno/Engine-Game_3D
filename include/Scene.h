#pragma once

#include "SceneNode.h"
#include "Shader.h"
#include "Logger.h"
#include <memory>

class Scene {
public:
    Scene() { 
        root = std::make_shared<SceneNode>();
        Logger::Info("[Scene] Created with an empty root node.");
    }
    
    void Update() {
        Logger::Debug("[Scene] Updating");
        root->Update();
    }
    
    void Render(const Shader &shader) {
        Logger::Debug("[Scene] Rendering");
        root->Render(shader);
    }
    
    std::shared_ptr<SceneNode> GetRoot() {
        return root;
    }
    
private:
    std::shared_ptr<SceneNode> root;
};