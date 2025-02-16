#ifndef SCENENODE_H
#define SCENENODE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Logger.h"
#include "Shader.h"  // Ensure to include the Shader header

/// Base class for scene nodes.
class SceneNode {
public:
    glm::mat4 localTransform;  // Local transformation matrix.
    glm::mat4 globalTransform; // Global transformation matrix computed recursively.

    // Constructor: Initializes both local and global transforms to identity.
    SceneNode() 
        : localTransform(1.0f), globalTransform(1.0f) { 
        Logger::Debug("[SceneNode] SceneNode created with identity transforms.");
    }

    virtual ~SceneNode() { }

    // Adds a child node to this scene node.
    void AddChild(const std::shared_ptr<SceneNode>& child) {
        children.push_back(child);
        Logger::Debug("[SceneNode] Child added to scene node. Total children: " + std::to_string(children.size()));
    }

    /// Recursively updates the global transformation based on the parent's transform.
    /// @param parentTransform The transformation matrix from the parent node.
    virtual void Update(const glm::mat4& parentTransform = glm::mat4(1.0f)) {
        globalTransform = parentTransform * localTransform;
        Logger::Debug("[SceneNode::Update] Global transform updated.");
        for (auto& child : children) {
            child->Update(globalTransform);
        }
    }

    /// Renders this node and all its child nodes.
    /// @param shader The shader used for rendering.
    virtual void Render(const Shader &shader) {
        Logger::Debug("[SceneNode::Render] Rendering node with " + std::to_string(children.size()) + " children.");
        for (auto& child : children) {
            child->Render(shader);
        }
    }

protected:
    std::vector<std::shared_ptr<SceneNode>> children; // List of child nodes.
};

#endif // SCENENODE_H