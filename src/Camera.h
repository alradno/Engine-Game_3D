#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Logger.h" // Use Logger for detailed logging instead of std::cout
#include <string>

class Camera {
public:
    glm::vec3 Position;         // Camera position in world space.
    glm::vec3 Front;            // Direction vector the camera is facing.
    glm::vec3 Up;               // Up vector of the camera.
    float Yaw;                  // Rotation around the Y-axis.
    float Pitch;                // Rotation around the X-axis.
    float MouseSensitivity;     // Sensitivity factor for mouse movement.

    // Constructor: Initializes camera position, orientation, and movement sensitivity.
    Camera(glm::vec3 position = glm::vec3(0.0f, 2.0f, 5.0f),
           glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f, float sensitivity = 0.1f)
        : Position(position), Front(front), Up(up), Yaw(yaw), Pitch(pitch),
          MouseSensitivity(sensitivity)
    {
        Logger::Info("[Camera] Constructor called. Initializing camera with default values.");
        Logger::Debug("[Camera] Initial Position: (" + std::to_string(Position.x) + ", " + std::to_string(Position.y) + ", " + std::to_string(Position.z) + ")");
        Logger::Debug("[Camera] Initial Front: (" + std::to_string(Front.x) + ", " + std::to_string(Front.y) + ", " + std::to_string(Front.z) + ")");
        Logger::Debug("[Camera] Initial Up: (" + std::to_string(Up.x) + ", " + std::to_string(Up.y) + ", " + std::to_string(Up.z) + ")");
        Logger::Debug("[Camera] Yaw: " + std::to_string(Yaw) + ", Pitch: " + std::to_string(Pitch) + ", MouseSensitivity: " + std::to_string(MouseSensitivity));
    }
    
    // Computes and returns the view matrix based on the camera's position and orientation.
    glm::mat4 GetViewMatrix() const {
        Logger::Debug("[Camera] Calculating view matrix using Position and Front vectors.");
        Logger::Debug("[Camera] Position: (" + std::to_string(Position.x) + ", " + std::to_string(Position.y) + ", " + std::to_string(Position.z) + ")");
        Logger::Debug("[Camera] Front: (" + std::to_string(Front.x) + ", " + std::to_string(Front.y) + ", " + std::to_string(Front.z) + ")");
        Logger::Debug("[Camera] Up: (" + std::to_string(Up.x) + ", " + std::to_string(Up.y) + ", " + std::to_string(Up.z) + ")");
        return glm::lookAt(Position, Position + Front, Up);
    }
    
    // Processes keyboard input and updates the camera position accordingly.
    // 'direction' should be one of: 'W', 'A', 'S', 'D'.
    // 'deltaTime' is the time elapsed since the last frame.
    void ProcessKeyboard(char direction, float deltaTime) {
        Logger::Debug("[Camera] Processing keyboard input. Direction: " + std::string(1, direction) + ", Delta Time: " + std::to_string(deltaTime));
        float velocity = 2.5f * deltaTime;
        if (direction == 'W') {
            Position += Front * velocity;
            Logger::Debug("[Camera] Moved forward. New Position: (" + std::to_string(Position.x) + ", " + std::to_string(Position.y) + ", " + std::to_string(Position.z) + ")");
        }
        if (direction == 'S') {
            Position -= Front * velocity;
            Logger::Debug("[Camera] Moved backward. New Position: (" + std::to_string(Position.x) + ", " + std::to_string(Position.y) + ", " + std::to_string(Position.z) + ")");
        }
        if (direction == 'A') {
            glm::vec3 left = glm::normalize(glm::cross(Front, Up));
            Position -= left * velocity;
            Logger::Debug("[Camera] Moved left. New Position: (" + std::to_string(Position.x) + ", " + std::to_string(Position.y) + ", " + std::to_string(Position.z) + ")");
        }
        if (direction == 'D') {
            glm::vec3 right = glm::normalize(glm::cross(Front, Up));
            Position += right * velocity;
            Logger::Debug("[Camera] Moved right. New Position: (" + std::to_string(Position.x) + ", " + std::to_string(Position.y) + ", " + std::to_string(Position.z) + ")");
        }
    }
    
    // Processes mouse movement input to update the camera's orientation.
    // 'xoffset' and 'yoffset' represent the change in mouse position.
    void ProcessMouseMovement(float xoffset, float yoffset) {
        Logger::Debug("[Camera] Processing mouse movement. X offset: " + std::to_string(xoffset) + ", Y offset: " + std::to_string(yoffset));
        // Apply sensitivity factor to mouse input.
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        Yaw += xoffset;
        Pitch += yoffset;
        Logger::Debug("[Camera] Updated Yaw: " + std::to_string(Yaw) + ", Updated Pitch: " + std::to_string(Pitch));
        
        // Clamp the pitch to prevent screen flip.
        if (Pitch > 89.0f) {
            Pitch = 89.0f;
            Logger::Debug("[Camera] Clamped Pitch to 89.0f (upper limit).");
        }
        if (Pitch < -89.0f) {
            Pitch = -89.0f;
            Logger::Debug("[Camera] Clamped Pitch to -89.0f (lower limit).");
        }
        UpdateCameraVectors();
    }
    
private:
    // Recalculates the Front vector based on the current Yaw and Pitch angles.
    void UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Logger::Debug("[Camera] Recalculated Front vector: (" + std::to_string(Front.x) + ", " + std::to_string(Front.y) + ", " + std::to_string(Front.z) + ")");
    }
};

#endif // CAMERA_H