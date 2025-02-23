#pragma once

#include "core/Coordinator.h"
#include "components/TransformComponent.h"
#include "utils/Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath> // Para fmod

/**
 * @class ECSPlayerController
 * @brief Controlador para mover un modelo estilo coche.
 *
 * Separamos el offset base (definido en el YAML) del giro dinámico, pero en lugar de
 * simplemente sumar la entrada, usamos una variable de velocidad angular para simular
 * aceleración y damping.
 *
 * La orientación efectiva se calcula como:
 *
 *     effectiveYaw = baseYawOffset + dynamicYaw
 *
 * donde dynamicYaw se integra a partir de la velocidad angular.
 */
class ECSPlayerController {
public:
    /**
     * @param coordinator Puntero al coordinator ECS.
     * @param entity La entidad del coche.
     * @param invertControls Si es true, se invierte la entrada de giro.
     */
    ECSPlayerController(Coordinator* coordinator, ECS::Entity entity, bool invertControls = true)
        : mCoordinator(coordinator),
          mEntity(entity),
          m_MoveSpeed(20.0f),
          m_MaxAngularSpeed(600.0f),
          m_Damping(5.0f),
          m_InvertControls(invertControls),
          m_DynamicYaw(0.0f),
          m_AngularVelocity(0.0f)
    {
        auto& transform = mCoordinator->GetComponent<TransformComponent>(mEntity);
        m_BaseYawOffset = transform.rotation.y;
        Logger::Debug("[ECSPlayerController] Constructor: baseYawOffset = " + std::to_string(m_BaseYawOffset));
        float effectiveYaw = m_BaseYawOffset + m_DynamicYaw;
        transform.rotation.y = effectiveYaw;
        transform.UpdateTransform();
    }
    
    void Update(float dt) {
        GLFWwindow* window = glfwGetCurrentContext();
        if (!window) return;
        auto& transform = mCoordinator->GetComponent<TransformComponent>(mEntity);
        
        // --- Procesamiento de la entrada de giro ---
        float turnInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            turnInput -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            turnInput += 1.0f;
        if (m_InvertControls)
            turnInput = -turnInput;
        // Se usa ThresholdLog para valores numéricos que varían con cada frame.
        Logger::ThresholdLog<float>("ECSPlayerController_turnInput", turnInput, 0.01f, LogLevel::DEBUG,
            "[ECSPlayerController] turnInput = " + std::to_string(turnInput), 0.5);
        
        // Calculamos aceleración angular
        float angularAcceleration = turnInput * m_MaxAngularSpeed;
        m_AngularVelocity += angularAcceleration * dt;
        m_AngularVelocity -= m_AngularVelocity * m_Damping * dt;
        if (m_AngularVelocity > m_MaxAngularSpeed)
            m_AngularVelocity = m_MaxAngularSpeed;
        if (m_AngularVelocity < -m_MaxAngularSpeed)
            m_AngularVelocity = -m_MaxAngularSpeed;
        Logger::ThresholdLog<float>("ECSPlayerController_AngularVelocity", m_AngularVelocity, 0.01f, LogLevel::DEBUG,
            "[ECSPlayerController] Angular velocity = " + std::to_string(m_AngularVelocity), 0.5);
        
        // Integración de la velocidad angular
        m_DynamicYaw += m_AngularVelocity * dt;
        m_DynamicYaw = fmod(m_DynamicYaw, 360.0f);
        Logger::ThresholdLog<float>("ECSPlayerController_dynamicYaw", m_DynamicYaw, 0.01f, LogLevel::DEBUG,
            "[ECSPlayerController] dynamicYaw = " + std::to_string(m_DynamicYaw), 0.5);
        
        float effectiveYaw = m_BaseYawOffset + m_DynamicYaw;
        Logger::ThresholdLog<float>("ECSPlayerController_effectiveYaw", effectiveYaw, 0.01f, LogLevel::DEBUG,
            "[ECSPlayerController] effectiveYaw = " + std::to_string(effectiveYaw), 0.5);
        transform.rotation.y = effectiveYaw;
        
        // --- Cálculo del vector forward ---
        float yawRadians = glm::radians(effectiveYaw);
        glm::vec3 forward;
        forward.x = sin(yawRadians);
        forward.y = 0.0f;
        forward.z = cos(yawRadians);
        forward = glm::normalize(forward);
        Logger::ThrottledLog("ECSPlayerController_forward", LogLevel::DEBUG,
            "[ECSPlayerController] forward vector = (" +
            std::to_string(forward.x) + ", " +
            std::to_string(forward.y) + ", " +
            std::to_string(forward.z) + ")", 0.5);
        
        // --- Procesamiento del movimiento ---
        float moveInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            moveInput += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            moveInput -= 1.0f;
        Logger::ThrottledLog("ECSPlayerController_moveInput", LogLevel::DEBUG,
            "[ECSPlayerController] moveInput = " + std::to_string(moveInput), 0.5);
        
        Logger::ThrottledLog("ECSPlayerController_translationAntes", LogLevel::DEBUG,
            "[ECSPlayerController] translation antes = (" +
            std::to_string(transform.translation.x) + ", " +
            std::to_string(transform.translation.y) + ", " +
            std::to_string(transform.translation.z) + ")", 0.5);
        
        transform.translation += forward * moveInput * m_MoveSpeed * dt;
        
        Logger::ThrottledLog("ECSPlayerController_translationDespues", LogLevel::DEBUG,
            "[ECSPlayerController] translation después = (" +
            std::to_string(transform.translation.x) + ", " +
            std::to_string(transform.translation.y) + ", " +
            std::to_string(transform.translation.z) + ")", 0.5);
        
        transform.UpdateTransform();
    }
    
private:
    Coordinator* mCoordinator;
    ECS::Entity mEntity;
    float m_MoveSpeed;        
    float m_MaxAngularSpeed;  
    float m_Damping;          
    bool m_InvertControls;    
    float m_BaseYawOffset;    
    float m_DynamicYaw;       
    float m_AngularVelocity;  
};