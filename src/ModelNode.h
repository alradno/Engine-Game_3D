#ifndef MODELNODE_H
#define MODELNODE_H

#include "SceneNode.h"
#include "Model.h"
#include "Logger.h"
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Shader.h"

/// Nodo de escena que encapsula un modelo 3D.
class ModelNode : public SceneNode {
public:
    std::shared_ptr<Model> model;
    
    ModelNode(const std::shared_ptr<Model>& m) : model(m) {
        Logger::Info("[ModelNode] ModelNode created.");
    }
    
    /// Renderiza el nodo enviando la transformación global al shader y luego dibujando el modelo.
    virtual void Render(const Shader &shader) override {
        Logger::Debug("[ModelNode] Rendering model node.");
        if (model) {
            // Enviar la transformación global al shader mediante la uniforme "model"
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(globalTransform));
            model->Draw();  // Se asume que el shader actual ya está en uso y las uniformes globales se han actualizado.
        } else {
            Logger::Warning("[ModelNode] No model to render.");
        }
        // Renderizar los hijos (si existen)
        SceneNode::Render(shader);
    }
};

#endif // MODELNODE_H
