/**
 * @file ModelLoaderHelper.cpp
 * @brief Implementation of the helper function to load and transform a glTF model.
 */

 #include "ModelLoaderHelper.h"
 #include "ResourceManager.h"
 #include "Logger.h"
 #include <glm/gtc/matrix_transform.hpp>
 
 std::shared_ptr<ModelNode> loadModel(const std::string& filePath,
                                        const glm::vec3& translation,
                                        const glm::vec3& rotation,
                                        const glm::vec3& scale)
 {
     // Load the model using ResourceManager. Here, the file path is used as the key.
     auto model = ResourceManager::LoadModel(filePath.c_str(), filePath);
 
     // Create a new ModelNode with the loaded model.
     auto node = std::make_shared<ModelNode>(model);
 
     // Construct the transformation matrix.
     // Start with an identity matrix.
     glm::mat4 transform = glm::mat4(1.0f);
     // Apply translation.
     transform = glm::translate(transform, translation);
     // Apply rotation (assuming Euler angles in degrees).
     transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
     transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
     transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
     // Apply scaling.
     transform = glm::scale(transform, scale);
 
     // Set the node's local transform.
     node->localTransform = transform;
 
     Logger::Info("[ModelLoaderHelper] Model loaded and transformed from file: " + filePath);
 
     return node;
 } 