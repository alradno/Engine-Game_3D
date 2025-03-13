#pragma once
#include <string>
#include <glm/glm.hpp>

struct TextComponent {
    std::string content;   // Contenido del texto a renderizar.
    std::string font;      // Ruta a la fuente, por ejemplo: "fonts/american_captain/American Captain.otf"
    int size;              // Tamaño de la fuente.
    glm::vec4 color;       // Color del texto (RGBA).

    // Constructor por defecto.
    TextComponent() 
        : content(""), font(""), size(32), color(1.0f, 1.0f, 1.0f, 1.0f) { }

    // Constructor parametrizado.
    TextComponent(const std::string& text, const std::string& fontPath, int fontSize, const glm::vec4& textColor)
        : content(text), font(fontPath), size(fontSize), color(textColor) { }
};