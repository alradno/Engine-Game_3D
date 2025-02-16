/**
 * @file PlayerController.cpp
 * @brief Implementation of the PlayerController class which processes input and updates the player and camera states.
 */

#include "PlayerController.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Logger.h"

PlayerController::PlayerController(SceneNode *player, Camera *camera, std::shared_ptr<ICameraControlStrategy> camStrategy)
    : m_Player(player), m_Camera(camera), m_MoveSpeed(5.0f),
      m_RotateSpeed(glm::radians(90.0f)), m_PlayerYaw(0.0f), m_camStrategy(camStrategy)
{
    m_BaseTransform = m_Player->localTransform;
    m_PlayerPosition = glm::vec3(m_BaseTransform[3]);
    Logger::Info("PlayerController: Initialized at position (" +
                 std::to_string(m_PlayerPosition.x) + ", " +
                 std::to_string(m_PlayerPosition.y) + ", " +
                 std::to_string(m_PlayerPosition.z) + ").");

    m_CameraOffset = glm::vec3(0.0f, 8.0f, 12.0f);
}

void PlayerController::Update(float dt)
{
    GLFWwindow *window = glfwGetCurrentContext();
    if (!window)
    {
        Logger::Error("PlayerController::Update: No current GLFW window found.");
        return;
    }

    bool moveForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool turnLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool turnRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    Logger::Debug("PlayerController: Input - W:" + std::to_string(moveForward) +
                  " S:" + std::to_string(moveBackward) +
                  " A:" + std::to_string(turnLeft) +
                  " D:" + std::to_string(turnRight));

    if (turnLeft)
        m_PlayerYaw += m_RotateSpeed * dt;
    if (turnRight)
        m_PlayerYaw -= m_RotateSpeed * dt;
    Logger::Debug("PlayerController: Updated yaw = " + std::to_string(m_PlayerYaw));

    glm::mat4 R = glm::rotate(glm::mat4(1.0f), m_PlayerYaw, glm::vec3(0, 1, 0));
    glm::mat3 dynamicRotation = glm::mat3(R);
    glm::vec3 forward = glm::normalize(dynamicRotation * glm::vec3(0, 0, 1));
    Logger::Debug("PlayerController: Computed forward vector = (" +
                  std::to_string(forward.x) + ", " +
                  std::to_string(forward.y) + ", " +
                  std::to_string(forward.z) + ")");

    if (moveForward)
        m_PlayerPosition += forward * m_MoveSpeed * dt;
    if (moveBackward)
        m_PlayerPosition -= forward * m_MoveSpeed * dt;
    Logger::Debug("PlayerController: Updated position = (" +
                  std::to_string(m_PlayerPosition.x) + ", " +
                  std::to_string(m_PlayerPosition.y) + ", " +
                  std::to_string(m_PlayerPosition.z) + ")");

    glm::mat4 T = glm::translate(glm::mat4(1.0f), m_PlayerPosition);
    m_Player->localTransform = T * R * m_BaseTransform;
    Logger::Info("PlayerController: Player transform updated.");

    // Notify observers about player movement
    Notify("PlayerMoved");

    // Update the camera using the injected strategy
    m_camStrategy->UpdateCamera(m_Camera, m_Player, dt);
    Logger::Info("PlayerController: Camera updated.");
}
