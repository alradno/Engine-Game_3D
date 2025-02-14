#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <glm/glm.hpp>
#include "SceneNode.h"
#include "Camera.h"

// The PlayerController class handles keyboard input to move and rotate the model (vehicle)
// and automatically updates the camera to keep the model framed in the shot. The camera's
// position is set using a fixed offset relative to the model's position, regardless of the model's rotation.
class PlayerController {
public:
    // Constructor: takes a pointer to the player node (vehicle model) and a pointer to the camera.
    // It initializes movement speeds, stores the model's base transform, and sets a default camera offset.
    PlayerController(SceneNode* player, Camera* camera);

    // Update is called every frame. It processes keyboard input to move and rotate the model,
    // then updates the camera's position and orientation so that the model remains centered.
    void Update(float dt);

private:
    SceneNode* m_Player;   // Pointer to the player node (vehicle model)
    Camera* m_Camera;      // Pointer to the camera

    float m_MoveSpeed;     // Movement speed (units per second)
    float m_RotateSpeed;   // Rotation speed (radians per second)

    glm::vec3 m_PlayerPosition; // The current position of the model, updated via input.
    float m_PlayerYaw;          // The current yaw (rotation around Y-axis), updated via input.

    glm::mat4 m_BaseTransform;  // The original transform of the model as set in main.

    // A fixed camera offset relative to the model's position. This offset is NOT rotated
    // by the model's rotation, so that the camera remains in a fixed position relative to the world.
    glm::vec3 m_CameraOffset;
};

#endif // PLAYERCONTROLLER_H
