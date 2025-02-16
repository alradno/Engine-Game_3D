/**
 * @file ModelLoaderHelper.h
 * @brief Helper to load a glTF model and apply a transformation (translation, rotation, scale).
 *
 * Usage example:
 *   auto node = loadModel("path/to/model.gltf",
 *                         glm::vec3(translation),
 *                         glm::vec3(rotation),   // rotation in degrees (Euler angles)
 *                         glm::vec3(scale));
 */

 #pragma once
 #include <memory>
 #include <string>
 #include <glm/glm.hpp>
 #include "ModelNode.h"  // ModelNode contains the Model pointer and the local transform
 
 /// Loads a glTF model from the given file path and applies the transformation.
 /// @param filePath The path to the glTF file.
 /// @param translation The translation vector.
 /// @param rotation The Euler rotation vector (in degrees).
 /// @param scale The scale vector.
 /// @return A shared pointer to a ModelNode with the model loaded and transformed.
 std::shared_ptr<ModelNode> loadModel(const std::string& filePath,
                                        const glm::vec3& translation,
                                        const glm::vec3& rotation,
                                        const glm::vec3& scale);
 