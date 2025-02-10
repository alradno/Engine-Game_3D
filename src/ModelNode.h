#ifndef MODELNODE_H
#define MODELNODE_H

#include "SceneNode.h"
#include "Model.h"
#include "Logger.h"
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Shader.h"

class ModelNode : public SceneNode {
public:
    std::shared_ptr<Model> model;
    
    ModelNode(const std::shared_ptr<Model>& m) : model(m) {
        Logger::Info("[ModelNode] ModelNode created.");
    }
    
    virtual void Render(const Shader &shader) override {
        Logger::Debug("[ModelNode] Rendering model node.");
        if (model) {
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(globalTransform));
            model->Draw(shader);
        } else {
            Logger::Warning("[ModelNode] No model to render.");
        }
        SceneNode::Render(shader);
    }
};

#endif
