#pragma once
#include <string>

struct ButtonComponent {
    std::string action;   // Acción asociada al botón ("play", "settings", "exit", etc.).

    // Constructor por defecto.
    ButtonComponent() 
        : action("") { }

    // Constructor parametrizado.
    ButtonComponent(const std::string& act)
        : action(act) { }
};