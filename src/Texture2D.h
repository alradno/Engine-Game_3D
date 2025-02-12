#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#include "stb_image.h"
#include "FileUtils.h"  // Now ImageData is located in FileUtils
#include "Logger.h"

class Texture2D {
public:
    unsigned int ID;       // OpenGL texture ID.
    int Width, Height;     // Dimensions of the texture.
    GLenum Internal_Format; // Internal format, e.g., GL_RGB or GL_SRGB_ALPHA for albedo.
    GLenum Image_Format;    // Image format, e.g., GL_RGB or GL_RGBA.

    // Wrapping and filtering options.
    GLenum Wrap_S;
    GLenum Wrap_T;
    GLenum Filter_Min;
    GLenum Filter_Mag;
    
    // Constructor: Initializes texture properties and generates a texture ID.
    Texture2D()
        : Width(0), Height(0),
          Internal_Format(GL_RGB), Image_Format(GL_RGB),
          Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT),
          Filter_Min(GL_LINEAR_MIPMAP_LINEAR), Filter_Mag(GL_LINEAR)
    {
        glGenTextures(1, &ID);
        Logger::Debug("[Texture2D] Generated texture ID: " + std::to_string(ID));
    }
    
    // Synchronous generation function (not recommended). Use GenerateFromData instead.
    void Generate(const char* file, bool alpha) {
        Logger::Warning("[Texture2D::Generate] Synchronous load called. Use GenerateFromData instead.");
    }
    
    // Generates the texture from the provided image data.
    void GenerateFromData(const FileUtils::ImageData& img, bool alpha) {
        if (img.data) {
            Width = img.width;
            Height = img.height;
            int desired_channels = alpha ? 4 : 3;
            GLenum format = (desired_channels == 4) ? GL_RGBA : GL_RGB;
            
            if (alpha) {
                Internal_Format = GL_SRGB_ALPHA;
                Image_Format = GL_RGBA;
                Wrap_S = GL_CLAMP_TO_EDGE;
                Wrap_T = GL_CLAMP_TO_EDGE;
            } else {
                Internal_Format = GL_RGB;
                Image_Format = GL_RGB;
            }
            
            glBindTexture(GL_TEXTURE_2D, ID);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    
            Logger::Info("[Texture2D::GenerateFromData] Image dimensions: " + std::to_string(Width) + "x" + std::to_string(Height) +
                         ", Channels: " + std::to_string(desired_channels) + ", Using format: " +
                         ((format == GL_RGBA) ? "GL_RGBA" : "GL_RGB"));
            
            glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, Width, Height, 0, format, GL_UNSIGNED_BYTE, img.data);
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                Logger::Error("[Texture2D::GenerateFromData] glTexImage2D error: " + std::to_string(err));
            }
            
            glGenerateMipmap(GL_TEXTURE_2D);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                Logger::Error("[Texture2D::GenerateFromData] glGenerateMipmap error: " + std::to_string(err));
            }
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_S);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_T);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_Min);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Mag);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                Logger::Error("[Texture2D::GenerateFromData] glTexParameteri error: " + std::to_string(err));
            }
            
            Logger::Info("[Texture2D::GenerateFromData] Texture generated successfully. Texture ID: " + std::to_string(ID));
            stbi_image_free(img.data);
        } else {
            Logger::Error("[Texture2D::GenerateFromData] Image data is null.");
        }
    }
};

#endif // TEXTURE2D_H