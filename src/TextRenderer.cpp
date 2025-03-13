#include "renderer/TextRenderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "engine/SceneResources.h"  // Asegúrate de incluir el header correspondiente
#include "utils/Logger.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height, SceneResources* sceneResources)
    : VAO(0), VBO(0)
{
    // Usar LoadShader de SceneResources para cargar el shader de texto.
    shader = sceneResources->LoadShader("text_vertex.glsl", "text_fragment.glsl", "textShader");
    if (!shader) {
        Logger::Error("[TextRenderer] No se pudo cargar el shader de texto.");
    }
    shader->Use();
    // Configurar la proyección ortográfica basada en píxeles.
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                      0.0f, static_cast<float>(height));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    // Asignar el sampler "text" a la unidad 0.
    glUniform1i(glGetUniformLocation(shader->ID, "text"), 0);

    // Configurar VAO/VBO para renderizar quads.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Cada vértice: x, y, texCoord.x, texCoord.y.
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Opcional: liberar las texturas de cada Character si es necesario.
}

void TextRenderer::Load(const std::string& font, unsigned int fontSize)
{
    Characters.clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        Logger::Error("[TextRenderer] ERROR: No se pudo inicializar FreeType");
        return;
    }
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face)) {
        Logger::Error("[TextRenderer] ERROR: No se pudo cargar la fuente: " + font);
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Desactivar restricciones de alineación.

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            Logger::Error("[TextRenderer] ERROR: No se pudo cargar el glifo para: " + std::to_string(c));
            continue;
        }
        // Generar una textura para el glifo.
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Configurar parámetros de la textura.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Almacenar el glifo en el mapa Characters.
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    Logger::Info("[TextRenderer] Se han cargado " + std::to_string(Characters.size()) + " caracteres.");
}

void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color)
{
    shader->Use();
    glUniform3f(glGetUniformLocation(shader->ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterar sobre cada caracter de la cadena.
    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // Definir los vértices para el quad de este caracter.
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // Actualizar VBO y dibujar el quad.
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Avanzar para el siguiente caracter (el avance se da en 1/64 de píxel).
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}