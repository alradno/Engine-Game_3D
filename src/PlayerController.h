#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <glm/glm.hpp>
#include "SceneNode.h"
#include "Camera.h"

// The PlayerController is responsible for handling the player's movement and rotation based on keyboard input.
// It updates the transformation of the player node (e.g., a car model) each frame.
// Note: Detailed logging is implemented in the source file (PlayerController.cpp).
class PlayerController {
public:
    // Constructor: Accepts pointers to the player node and the camera.
    // In this example, the camera remains fixed and is not updated by the controller.
    PlayerController(SceneNode* player, Camera* camera);

    // Updates the player's transformation based on input.
    // 'dt' represents the time elapsed (in seconds) since the last update.
    void Update(float dt);

private:
    SceneNode* m_Player;  // Pointer to the player node (e.g., the car model)
    Camera* m_Camera;     // Pointer to the camera (unused in this test)

    float m_MoveSpeed;    // Movement speed in units per second.
    float m_RotateSpeed;  // Rotation speed in radians per second.

    // Current state of the player.
    glm::vec3 m_PlayerPosition; // Updated position based on input.
    float m_PlayerYaw;          // Updated yaw (rotation around the Y-axis) based on input.

    // The base transformation (original transform) of the player model as set in main.
    glm::mat4 m_BaseTransform;
};

#endif // PLAYERCONTROLLER_H
