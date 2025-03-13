#pragma once
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <memory>
#include "renderer/Shader.h"

// Estructura para almacenar los datos de cada glifo.
struct Character {
    unsigned int TextureID; // ID de la textura del glifo.
    glm::ivec2 Size;        // Tamaño del glifo.
    glm::ivec2 Bearing;     // Offset desde la línea base hasta la parte superior izquierda del glifo.
    unsigned int Advance;   // Desplazamiento para el siguiente glifo.
};

class SceneResources; // Declaración adelantada.

class TextRenderer {
public:
    // Constructor: recibe el ancho y alto de la ventana (en píxeles) y un puntero a SceneResources.
    TextRenderer(unsigned int width, unsigned int height, SceneResources* sceneResources);
    ~TextRenderer();

    // Carga una fuente (archivo TTF) con un tamaño de píxel dado.
    void Load(const std::string& font, unsigned int fontSize);

    // Renderiza el texto en la posición (x, y) (en píxeles), con escala y color RGB.
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);

private:
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;
    std::shared_ptr<Shader> shader;
};