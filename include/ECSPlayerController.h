#ifndef ECSPLAYERCONTROLLER_H
#define ECSPLAYERCONTROLLER_H

#include "Coordinator.h"
#include "TransformComponent.h"
#include "Logger.h"
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
 * aceleración y damping. Así, al mantener pulsada la tecla, se acelera hasta un máximo,
 * y al soltarla la velocidad angular se amortigua.
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
          m_MaxAngularSpeed(600.0f),  // Máxima velocidad angular en grados/segundo
          m_Damping(5.0f),           // Factor de damping para desacelerar
          m_InvertControls(invertControls),
          m_DynamicYaw(0.0f),
          m_AngularVelocity(0.0f)
    {
        auto& transform = mCoordinator->GetComponent<TransformComponent>(mEntity);
        // Se guarda el offset base en yaw definido en el YAML.
        // Por ejemplo, en tu YAML: rotation: [-90.0, 180.0, 0.0] → baseYawOffset = 180°
        m_BaseYawOffset = transform.rotation.y;
        Logger::Debug("[ECSPlayerController] Constructor: baseYawOffset = " + std::to_string(m_BaseYawOffset));
        // Inicializamos la rotación efectiva
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
            turnInput -= 1.0f;  // Gira a la izquierda
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            turnInput += 1.0f;  // Gira a la derecha
        // Si se desea invertir la entrada, se aplica el flag
        if (m_InvertControls)
            turnInput = -turnInput;
        Logger::Debug("[ECSPlayerController] turnInput = " + std::to_string(turnInput));
        
        // Calculamos aceleración angular a partir de la entrada
        float angularAcceleration = turnInput * m_MaxAngularSpeed; // valor en grados/segundo²
        // Actualizamos la velocidad angular con aceleración
        m_AngularVelocity += angularAcceleration * dt;
        // Aplicamos damping para amortiguar cuando no hay entrada
        m_AngularVelocity -= m_AngularVelocity * m_Damping * dt;
        // Limitamos la velocidad angular al máximo
        if (m_AngularVelocity > m_MaxAngularSpeed)
            m_AngularVelocity = m_MaxAngularSpeed;
        if (m_AngularVelocity < -m_MaxAngularSpeed)
            m_AngularVelocity = -m_MaxAngularSpeed;
        Logger::Debug("[ECSPlayerController] Angular velocity = " + std::to_string(m_AngularVelocity));
        
        // Integramos la velocidad angular para actualizar el ángulo dinámico
        m_DynamicYaw += m_AngularVelocity * dt;
        // Normalizamos dynamicYaw para evitar valores excesivos
        m_DynamicYaw = fmod(m_DynamicYaw, 360.0f);
        Logger::Debug("[ECSPlayerController] dynamicYaw = " + std::to_string(m_DynamicYaw));
        
        // Calculamos el yaw efectivo combinando el offset base y el ángulo dinámico
        float effectiveYaw = m_BaseYawOffset + m_DynamicYaw;
        Logger::Debug("[ECSPlayerController] effectiveYaw = " + std::to_string(effectiveYaw));
        transform.rotation.y = effectiveYaw;
        
        // --- Cálculo del vector forward ---
        float yawRadians = glm::radians(effectiveYaw);
        glm::vec3 forward;
        // Con effectiveYaw = 180° y dynamicYaw = 0, forward = (0, 0, -1) (según la convención que deseas)
        forward.x = sin(yawRadians);
        forward.y = 0.0f;
        forward.z = cos(yawRadians);
        forward = glm::normalize(forward);
        Logger::Debug("[ECSPlayerController] forward vector = (" +
                      std::to_string(forward.x) + ", " +
                      std::to_string(forward.y) + ", " +
                      std::to_string(forward.z) + ")");
        
        // --- Procesamiento del movimiento ---
        float moveInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            moveInput += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            moveInput -= 1.0f;
        Logger::Debug("[ECSPlayerController] moveInput = " + std::to_string(moveInput));
        
        Logger::Debug("[ECSPlayerController] translation antes = (" +
                      std::to_string(transform.translation.x) + ", " +
                      std::to_string(transform.translation.y) + ", " +
                      std::to_string(transform.translation.z) + ")");
        transform.translation += forward * moveInput * m_MoveSpeed * dt;
        Logger::Debug("[ECSPlayerController] translation después = (" +
                      std::to_string(transform.translation.x) + ", " +
                      std::to_string(transform.translation.y) + ", " +
                      std::to_string(transform.translation.z) + ")");
        
        // Actualizamos la transformación final (T * R * S).
        transform.UpdateTransform();
    }
    
private:
    Coordinator* mCoordinator;
    ECS::Entity mEntity;
    float m_MoveSpeed;        // Velocidad de movimiento (unidades/segundo)
    float m_MaxAngularSpeed;  // Máxima velocidad angular en grados/segundo
    float m_Damping;          // Factor de damping para desacelerar la velocidad angular
    bool m_InvertControls;    // Si es true, se invierte la entrada de giro
    float m_BaseYawOffset;    // Offset base proveniente del YAML (por ejemplo, 180°)
    float m_DynamicYaw;       // Ángulo dinámico de giro, acumulado a partir de la velocidad angular
    float m_AngularVelocity;  // Velocidad angular actual (grados/segundo)
};

#endif // ECSPLAYERCONTROLLER_H