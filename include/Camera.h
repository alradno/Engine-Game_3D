#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Logger.h"

class Camera {
public:
    glm::vec3 Position{0.0f, 2.0f, 5.0f};
    glm::vec3 Front{0.0f, 0.0f, -1.0f};
    glm::vec3 Up{0.0f, 1.0f, 0.0f};
    float Yaw{-90.0f};
    float Pitch{0.0f};
    float MouseSensitivity{0.1f};

    Camera() {
        Logger::Info("[Camera] Default constructor");
        UpdateCameraVectors();
    }
    
    Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float yaw, float pitch, float sensitivity)
        : Position(pos), Front(front), Up(up), Yaw(yaw), Pitch(pitch), MouseSensitivity(sensitivity)
    {
        Logger::Info("[Camera] Custom constructor");
        UpdateCameraVectors();
    }
    
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }
    
    void ProcessKeyboard(char direction, float deltaTime) {
        float velocity = 2.5f * deltaTime;
        if (direction == 'W') {
            Position += Front * velocity;
            Logger::Debug("[Camera] Moved forward");
        } else if (direction == 'S') {
            Position -= Front * velocity;
            Logger::Debug("[Camera] Moved backward");
        } else if (direction == 'A') {
            glm::vec3 left = glm::normalize(glm::cross(Front, Up));
            Position -= left * velocity;
            Logger::Debug("[Camera] Moved left");
        } else if (direction == 'D') {
            glm::vec3 right = glm::normalize(glm::cross(Front, Up));
            Position += right * velocity;
            Logger::Debug("[Camera] Moved right");
        }
    }
    
    void ProcessMouseMovement(float xoffset, float yoffset) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        Yaw += xoffset;
        Pitch += yoffset;
        if (Pitch > 89.0f)  Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
        UpdateCameraVectors();
        Logger::Debug("[Camera] Updated orientation from mouse movement");
    }
    
private:
    void UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
    }
};