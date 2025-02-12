#include "PlayerController.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// Constructor: Stores the player's base transform and initial position.
// Also logs the initial position for debugging purposes.
PlayerController::PlayerController(SceneNode* player, Camera* camera)
    : m_Player(player), m_Camera(camera), m_MoveSpeed(5.0f),
      m_RotateSpeed(glm::radians(90.0f)), m_PlayerYaw(0.0f)
{
    // Save the initial local transform of the player.
    m_BaseTransform = m_Player->localTransform;
    // Extract the initial position from the transform's translation column.
    m_PlayerPosition = glm::vec3(m_BaseTransform[3]);
    Logger::Info("[PlayerController] Initialized with base position (" +
                 std::to_string(m_PlayerPosition.x) + ", " +
                 std::to_string(m_PlayerPosition.y) + ", " +
                 std::to_string(m_PlayerPosition.z) + ").");
}

void PlayerController::Update(float dt) {
    // Retrieve the current GLFW window context.
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        Logger::Error("[PlayerController::Update] No current GLFW window.");
        return;
    }

    // Poll keyboard input for movement and rotation.
    bool moveForward  = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool turnLeft     = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool turnRight    = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    Logger::Debug("[PlayerController::Update] Input - W:" + std::to_string(moveForward) +
                  " S:" + std::to_string(moveBackward) +
                  " A:" + std::to_string(turnLeft) +
                  " D:" + std::to_string(turnRight));

    // Update the player's yaw angle based on rotation input.
    if (turnLeft)
        m_PlayerYaw += m_RotateSpeed * dt;
    if (turnRight)
        m_PlayerYaw -= m_RotateSpeed * dt;

    Logger::Debug("[PlayerController::Update] Updated yaw = " + std::to_string(m_PlayerYaw));

    // Construct the rotation matrix from the updated yaw.
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), m_PlayerYaw, glm::vec3(0, 1, 0));
    // Calculate the forward vector by applying the rotation to the default forward direction (0, 0, 1).
    glm::mat3 rotation3x3 = glm::mat3(rotationMatrix);
    glm::vec3 forward = glm::normalize(rotation3x3 * glm::vec3(0, 0, 1));

    Logger::Debug("[PlayerController::Update] Forward vector = (" +
                  std::to_string(forward.x) + ", " +
                  std::to_string(forward.y) + ", " +
                  std::to_string(forward.z) + ")");

    // Update the player's position based on forward/backward movement.
    if (moveForward)
        m_PlayerPosition += forward * m_MoveSpeed * dt;
    if (moveBackward)
        m_PlayerPosition -= forward * m_MoveSpeed * dt;

    Logger::Debug("[PlayerController::Update] Updated position = (" +
                  std::to_string(m_PlayerPosition.x) + ", " +
                  std::to_string(m_PlayerPosition.y) + ", " +
                  std::to_string(m_PlayerPosition.z) + ")");

    // Construct the translation matrix from the updated position.
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_PlayerPosition);
    // Update the player's transformation by combining translation, rotation, and the base transform.
    m_Player->localTransform = translationMatrix * rotationMatrix * m_BaseTransform;
    Logger::Info("[PlayerController::Update] Player transform updated.");

    // Note: For this test, the camera remains fixed.
}
