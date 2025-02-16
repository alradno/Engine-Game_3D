#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Logger.h"
#include "Shader.h"

class SceneNode {
public:
    glm::mat4 localTransform{1.0f};
    glm::mat4 globalTransform{1.0f};

    SceneNode() {
        Logger::Debug("[SceneNode] Created with identity transforms");
    }
    
    virtual ~SceneNode() = default;
    
    void AddChild(const std::shared_ptr<SceneNode>& child) {
        children.push_back(child);
        Logger::Debug("[SceneNode] Child added. Total children: " + std::to_string(children.size()));
    }
    
    virtual void Update(const glm::mat4& parentTransform = glm::mat4(1.0f)) {
        globalTransform = parentTransform * localTransform;
        for (auto& child : children)
            child->Update(globalTransform);
    }
    
    virtual void Render(const Shader &shader) {
        for (auto& child : children)
            child->Render(shader);
    }
    
protected:
    std::vector<std::shared_ptr<SceneNode>> children;
};