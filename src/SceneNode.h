#ifndef SCENENODE_H
#define SCENENODE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Logger.h"
#include "Shader.h"  // Asegurarse de incluir el header de Shader

/// Clase base para los nodos de la escena.
class SceneNode {
public:
    glm::mat4 localTransform;
    glm::mat4 globalTransform;

    SceneNode() 
        : localTransform(1.0f), globalTransform(1.0f) { }

    virtual ~SceneNode() { }

    void AddChild(const std::shared_ptr<SceneNode>& child) {
        children.push_back(child);
        Logger::Debug("[SceneNode] Child added.");
    }

    /// Actualiza la transformación global recursivamente.
    virtual void Update(const glm::mat4& parentTransform = glm::mat4(1.0f)) {
        globalTransform = parentTransform * localTransform;
        for (auto& child : children) {
            child->Update(globalTransform);
        }
    }

    /// Renderiza este nodo y sus hijos.
    virtual void Render(const Shader &shader) {
        for (auto& child : children) {
            child->Render(shader);
        }
    }

protected:
    std::vector<std::shared_ptr<SceneNode>> children;
};

#endif // SCENENODE_H
