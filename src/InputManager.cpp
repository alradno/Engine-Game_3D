#include "core/InputManager.h"
#include <iostream>

GLFWwindow* InputManager::window = nullptr;

void InputManager::SetWindow(GLFWwindow* win) {
    window = win;
}

bool InputManager::IsMouseButtonPressed(int button) {
    if (!window) {
        std::cerr << "InputManager: Ventana no asignada!" << std::endl;
        return false;
    }
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

glm::vec2 InputManager::GetMousePosition() {
    glm::vec2 pos(0.0f, 0.0f);
    if (!window)
        return pos;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // Convertir posición de píxeles a coordenadas normalizadas en el rango [-1, 1].
    float x = (xpos / static_cast<float>(width)) * 2.0f - 1.0f;
    // Invertir Y ya que la posición en ventana tiene el origen en la esquina superior izquierda.
    float y = 1.0f - (ypos / static_cast<float>(height)) * 2.0f;
    pos.x = x;
    pos.y = y;
    return pos;
}

bool InputManager::IsKeyPressed(int key) {
    if (!window) return false;
    return glfwGetKey(window, key) == GLFW_PRESS;
}