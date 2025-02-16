/**
 * @file PlayerController.h
 * @brief Declaration of the PlayerController class that handles player input and updates the player state.
 *        Inherits from Subject to notify observers about events.
 */

#pragma once
#include "SceneNode.h"
#include "Camera.h"
#include "Subject.h"
#include "ICameraControlStrategy.h"
#include <glm/glm.hpp>
#include <memory>

class PlayerController : public Subject
{
public:
    PlayerController(SceneNode *player, Camera *camera, std::shared_ptr<ICameraControlStrategy> camStrategy);
    void Update(float dt);

private:
    SceneNode *m_Player;
    Camera *m_Camera;
    float m_MoveSpeed;
    float m_RotateSpeed;
    glm::vec3 m_PlayerPosition;
    float m_PlayerYaw;
    glm::mat4 m_BaseTransform;
    glm::vec3 m_CameraOffset;
    std::shared_ptr<ICameraControlStrategy> m_camStrategy;
};
