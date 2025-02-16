#ifndef MODELNODE_H
#define MODELNODE_H

#include "SceneNode.h"
#include "Model.h"
#include "Logger.h"
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Shader.h"

// Scene node that encapsulates a 3D model.
class ModelNode : public SceneNode {
public:
    // Pointer to the 3D model associated with this node.
    std::shared_ptr<Model> model;
    
    // Constructor: Initializes the ModelNode with the given model.
    ModelNode(const std::shared_ptr<Model>& m) : model(m) {
        Logger::Info("[ModelNode] ModelNode created.");
    }
    
    // Renders the node by sending the global transformation to the shader
    // and then drawing the model.
    virtual void Render(const Shader &shader) override {
        Logger::Debug("[ModelNode] Rendering model node.");
        if (model) {
            // Retrieve the location of the "model" uniform in the shader.
            GLint modelLoc = glGetUniformLocation(shader.ID, "model");
            if (modelLoc == -1) {
                Logger::Warning("[ModelNode] Shader uniform 'model' not found.");
            } else {
                // Send the global transformation matrix to the shader uniform "model".
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(globalTransform));
                Logger::Debug("[ModelNode] Global transformation matrix sent to shader uniform 'model'.");
            }
            
            // Draw the model. It is assumed that the shader is already in use
            // and that all necessary global uniforms have been updated.
            model->Draw();
        } else {
            Logger::Warning("[ModelNode] No model to render.");
        }
        
        // Render any child nodes.
        SceneNode::Render(shader);
    }
};

#endif // MODELNODE_H