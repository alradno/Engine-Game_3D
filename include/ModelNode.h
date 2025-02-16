#pragma once

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
    
    explicit ModelNode(const std::shared_ptr<Model>& m) : model(m) {
        Logger::Info("[ModelNode] Created");
    }
    
    virtual void Render(const Shader &shader) override {
        Logger::Debug("[ModelNode] Rendering");
        if (model) {
            GLint modelLoc = glGetUniformLocation(shader.ID, "model");
            if (modelLoc == -1)
                Logger::Warning("[ModelNode] 'model' uniform not found");
            else
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(globalTransform));
            model->Draw();
        } else
            Logger::Warning("[ModelNode] No model to render");
        
        SceneNode::Render(shader);
    }
};