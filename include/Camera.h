#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Logger.h"

class Camera {
public:
    glm::vec3 Position{0.0f, 2.0f, 5.0f};
    glm::vec3 Front{0.0f, 0.0f, -1.0f};
    glm::vec3 Up{0.0f, 1.0f, 0.0f};
    float Yaw{0.0f};
    float Pitch{0.0f};
    float MouseSensitivity{0.1f};

    // Nueva bandera para evitar que la cámara se actualice con input.
    bool fixedCamera = true;

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
    
    // Procesamiento del teclado: si la cámara está en modo fijo, no se actualiza.
    void ProcessKeyboard(char direction, float deltaTime) {
        if (fixedCamera) return;
        float velocity = 2.5f * deltaTime;
        if (direction == 'W') {
            Position += Front * velocity;
            Logger::ThrottledLog("Camera_MovedForward", LogLevel::DEBUG, "[Camera] Moved forward", 0.5);
        } else if (direction == 'S') {
            Position -= Front * velocity;
            Logger::ThrottledLog("Camera_MovedBackward", LogLevel::DEBUG, "[Camera] Moved backward", 0.5);
        } else if (direction == 'A') {
            glm::vec3 left = glm::normalize(glm::cross(Front, Up));
            Position -= left * velocity;
            Logger::ThrottledLog("Camera_MovedLeft", LogLevel::DEBUG, "[Camera] Moved left", 0.5);
        } else if (direction == 'D') {
            glm::vec3 right = glm::normalize(glm::cross(Front, Up));
            Position += right * velocity;
            Logger::ThrottledLog("Camera_MovedRight", LogLevel::DEBUG, "[Camera] Moved right", 0.5);
        }
    }
    
    // Procesamiento del movimiento del mouse: si la cámara está fija, no se actualiza.
    void ProcessMouseMovement(float xoffset, float yoffset) {
        if (fixedCamera) return;
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        Yaw += xoffset;
        Pitch += yoffset;
        if (Pitch > 89.0f)  Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
        UpdateCameraVectors();
        Logger::ThrottledLog("Camera_MouseMovement", LogLevel::DEBUG, "[Camera] Updated orientation from mouse movement", 0.5);
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