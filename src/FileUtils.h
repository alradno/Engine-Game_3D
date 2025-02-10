#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "stb_image.h"
#include "Logger.h"  // Se incluye el Logger

namespace FileUtils {

// Normaliza la ruta usando barras "/" y devuelve la ruta en formato genérico.
inline std::string NormalizePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.generic_string();
}

// Estructura para almacenar datos de imagen.
struct ImageData {
    unsigned char* data;
    int width;
    int height;
    int channels;
};

// Carga una imagen desde disco y la fuerza a un número fijo de canales.
// Si alpha es true, se fuerza 4 canales (RGBA); de lo contrario, 3 canales (RGB).
inline ImageData LoadImageData(const std::string& path, bool alpha) {
    ImageData img;
    
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        Logger::Error("[FileUtils::LoadImageData] Cannot open file: " + path);
        img.data = nullptr;
        img.width = img.height = img.channels = 0;
        return img;
    }
    auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    Logger::Info("[FileUtils::LoadImageData] File size: " + std::to_string(fileSize) + " bytes for " + path);
    file.close();
    
    stbi_set_flip_vertically_on_load(false);  // Ajustable según necesidad
    int desired_channels = alpha ? 4 : 3;
    img.data = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, desired_channels);
    if (!img.data) {
        Logger::Error("[FileUtils::LoadImageData] stbi_load failed for " + path + ". Reason: " + stbi_failure_reason());
    } else {
        // Forzamos el número deseado de canales
        img.channels = desired_channels;
        Logger::Info("[FileUtils::LoadImageData] Loaded image from: " + path +
                     " (Width: " + std::to_string(img.width) +
                     ", Height: " + std::to_string(img.height) +
                     ", Channels: " + std::to_string(img.channels) + ")");
    }
    return img;
}

} // namespace FileUtils

#endif // FILEUTILS_H