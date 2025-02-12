#ifndef SCENE_H
#define SCENE_H

#include "SceneNode.h"
#include "Shader.h"
#include "Logger.h"
#include <memory>

/// Class representing the complete scene.
class Scene {
public:
    // Constructor: Creates a scene with an empty root node.
    Scene() {
        root = std::make_shared<SceneNode>();
        Logger::Info("[Scene] Scene created with an empty root node.");
    }
    
    // Updates the scene by calling Update() on the root node.
    void Update() {
        Logger::Debug("[Scene] Updating scene...");
        root->Update();
        Logger::Debug("[Scene] Scene update completed.");
    }
    
    // Renders the scene by invoking Render() on the root node with the provided shader.
    void Render(const Shader &shader) {
        Logger::Debug("[Scene] Rendering scene using the provided shader...");
        root->Render(shader);
        Logger::Debug("[Scene] Scene render completed.");
    }
    
    // Returns the root node of the scene.
    std::shared_ptr<SceneNode> GetRoot() {
        Logger::Debug("[Scene] Retrieving root node.");
        return root;
    }
    
private:
    std::shared_ptr<SceneNode> root; // Root node of the scene graph.
};

#endif // SCENE_H