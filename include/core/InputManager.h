#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputManager {
public:
    // Asigna la ventana activa (esto se debe hacer en el main una vez creada la ventana)
    static void SetWindow(GLFWwindow* window);

    // Verifica si el botón del mouse (por ejemplo, GLFW_MOUSE_BUTTON_LEFT) está presionado.
    static bool IsMouseButtonPressed(int button);

    // Obtiene la posición actual del mouse en coordenadas normalizadas [-1, 1] para X e Y.
    static glm::vec2 GetMousePosition();

    // Verifica si una tecla está presionada.
    static bool IsKeyPressed(int key);

private:
    static GLFWwindow* window;
};