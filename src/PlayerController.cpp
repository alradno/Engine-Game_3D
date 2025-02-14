#include "PlayerController.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// Constructor: stores the model's base transform, initializes the model's position, and sets a fixed camera offset.
PlayerController::PlayerController(SceneNode* player, Camera* camera)
    : m_Player(player), m_Camera(camera), m_MoveSpeed(5.0f),
      m_RotateSpeed(glm::radians(90.0f)), m_PlayerYaw(0.0f)
{
    // Save the base transform (original translation, rotation, scale) of the model as set in main.
    m_BaseTransform = m_Player->localTransform;
    // Extract the initial position from the base transform.
    m_PlayerPosition = glm::vec3(m_BaseTransform[3]);
    Logger::Info("PlayerController: Initialized. Base position = (" +
                 std::to_string(m_PlayerPosition.x) + ", " +
                 std::to_string(m_PlayerPosition.y) + ", " +
                 std::to_string(m_PlayerPosition.z) + ").");
    
    // Set a fixed camera offset. This offset will remain constant regardless of model rotation.
    // For example, (0, 8, 12) places the camera 8 units above and 12 units behind the model.
    m_CameraOffset = glm::vec3(0.0f, 8.0f, 12.0f);
    Logger::Info("PlayerController: Camera offset set to (" +
                 std::to_string(m_CameraOffset.x) + ", " +
                 std::to_string(m_CameraOffset.y) + ", " +
                 std::to_string(m_CameraOffset.z) + ").");
}

void PlayerController::Update(float dt) {
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        Logger::Error("PlayerController::Update: No current GLFW window.");
        return;
    }

    // Read keyboard input for movement and rotation.
    bool moveForward  = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool turnLeft     = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool turnRight    = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    Logger::Debug("PlayerController::Update: Input states - W: " + std::to_string(moveForward) +
                  ", S: " + std::to_string(moveBackward) +
                  ", A: " + std::to_string(turnLeft) +
                  ", D: " + std::to_string(turnRight));

    // Update the model's yaw (rotation around Y-axis) based on left/right input.
    if (turnLeft)
        m_PlayerYaw += m_RotateSpeed * dt;
    if (turnRight)
        m_PlayerYaw -= m_RotateSpeed * dt;
    Logger::Debug("PlayerController::Update: Updated yaw = " + std::to_string(m_PlayerYaw));

    // Create a dynamic rotation matrix using the updated yaw.
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), m_PlayerYaw, glm::vec3(0, 1, 0));

    // Compute the forward vector by applying the rotation to the vector (0, 0, 1).
    // Using (0,0,1) ensures that pressing 'W' moves the model forward.
    glm::mat3 dynamicRotation = glm::mat3(R);
    glm::vec3 forward = dynamicRotation * glm::vec3(0, 0, 1);
    forward = glm::normalize(forward);
    Logger::Debug("PlayerController::Update: Computed forward vector = (" +
                  std::to_string(forward.x) + ", " +
                  std::to_string(forward.y) + ", " +
                  std::to_string(forward.z) + ")");

    // Update the model's position based on forward/backward input.
    if (moveForward)
        m_PlayerPosition += forward * m_MoveSpeed * dt;
    if (moveBackward)
        m_PlayerPosition -= forward * m_MoveSpeed * dt;
    Logger::Debug("PlayerController::Update: Updated model position = (" +
                  std::to_string(m_PlayerPosition.x) + ", " +
                  std::to_string(m_PlayerPosition.y) + ", " +
                  std::to_string(m_PlayerPosition.z) + ")");

    // Build the translation matrix using the updated position.
    glm::mat4 T = glm::translate(glm::mat4(1.0f), m_PlayerPosition);
    // Update the model's transformation by combining translation, rotation, and the base transform.
    m_Player->localTransform = T * R * m_BaseTransform;
    Logger::Info("PlayerController::Update: Model transform updated.");

    // --- Automatic Camera Follow Implementation ---
    // In this implementation, the camera will follow the model's position but maintain a fixed offset.
    // The fixed offset is not transformed by the model's rotation, so the camera's framing remains consistent.
    m_Camera->Position = m_PlayerPosition + m_CameraOffset;
    // Update the camera's front vector so that it always looks at the model.
    m_Camera->Front = glm::normalize(m_PlayerPosition - m_Camera->Position);
    // Keep the camera's up vector fixed.
    m_Camera->Up = glm::vec3(0, 1, 0);
    Logger::Info("PlayerController::Update: Camera updated to follow model. New Camera Position = (" +
                 std::to_string(m_Camera->Position.x) + ", " +
                 std::to_string(m_Camera->Position.y) + ", " +
                 std::to_string(m_Camera->Position.z) + "), Front = (" +
                 std::to_string(m_Camera->Front.x) + ", " +
                 std::to_string(m_Camera->Front.y) + ", " +
                 std::to_string(m_Camera->Front.z) + ").");
}
