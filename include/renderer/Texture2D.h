#pragma once

#include <glad/glad.h>
#include <string>
#include "stb_image.h"
#include "utils/FileUtils.h" // Define ImageData.
#include "utils/Logger.h"
#include "utils/GLDebug.h" // Para usar GLCall, etc.

class Texture2D
{
public:
    unsigned int ID = 0;
    int Width = 0, Height = 0;
    GLenum Internal_Format = GL_RGB;                                     // Formato interno
    GLenum Image_Format = GL_RGB;                                        // Formato de la imagen cargada
    GLenum Wrap_S = GL_REPEAT, Wrap_T = GL_REPEAT;                       // Wrapping
    GLenum Filter_Min = GL_LINEAR_MIPMAP_LINEAR, Filter_Mag = GL_LINEAR; // Filtros

    // Constructor: genera la textura en OpenGL y registra el ID.
    Texture2D()
    {
        GLCall(glGenTextures(1, &ID));
        Logger::Debug("[Texture2D] Generated ID: " + std::to_string(ID));
    }

    // Método no recomendado: carga síncrona de textura.
    void Generate(const char *file, bool alpha)
    {
        Logger::Warning("[Texture2D] Use GenerateFromData instead");
    }

    /**
     * @brief Genera la textura a partir de los datos de imagen proporcionados.
     * @param img Datos de la imagen (ancho, alto, canales y puntero a los datos).
     * @param alpha Indica si se espera que la imagen tenga canal alfa.
     */
    void GenerateFromData(const FileUtils::ImageData &img, bool alpha)
    {
        if (!img.data)
        {
            Logger::Error("[Texture2D] Image data is null");
            return;
        }
        Width = img.width;
        Height = img.height;
        int desired_channels = alpha ? 4 : 3;
        GLenum format = (desired_channels == 4) ? GL_RGBA : GL_RGB;

        // Configuración de formatos y parámetros según si se usa alfa.
        if (alpha)
        {
            Internal_Format = GL_SRGB_ALPHA;
            Image_Format = GL_RGBA;
            Wrap_S = GL_CLAMP_TO_EDGE;
            Wrap_T = GL_CLAMP_TO_EDGE;
        }
        else
        {
            Internal_Format = GL_RGB;
            Image_Format = GL_RGB;
        }

        GLCall(glBindTexture(GL_TEXTURE_2D, ID));
        GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, Width, Height, 0, format, GL_UNSIGNED_BYTE, img.data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_S));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_T));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_Min));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Mag));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));

        Logger::Info("[Texture2D] Texture generated (ID: " + std::to_string(ID) + ")");
        stbi_image_free(img.data);
    }
};