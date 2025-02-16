/**
 * @file FixedOffsetCameraStrategy.h
 * @brief Camera control strategy that maintains a fixed offset from the player.
 */

 #pragma once
 #include "ICameraControlStrategy.h"
 #include <glm/glm.hpp>
 
 class FixedOffsetCameraStrategy : public ICameraControlStrategy {
 public:
     explicit FixedOffsetCameraStrategy(const glm::vec3& offset) : m_offset(offset) {}
     
     void UpdateCamera(Camera* camera, const SceneNode* player, float dt) override {
         glm::vec3 playerPos = glm::vec3(player->globalTransform[3]);
         camera->Position = playerPos + m_offset;
         camera->Front = glm::normalize(playerPos - camera->Position);
         camera->Up = glm::vec3(0, 1, 0);
     }
 private:
     glm::vec3 m_offset;
 };
 