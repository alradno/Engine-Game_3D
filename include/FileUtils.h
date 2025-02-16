#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include "stb_image.h"
#include "Logger.h"

namespace FileUtils {

inline std::string NormalizePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    std::string normalized = fsPath.generic_string();
    Logger::Debug("[FileUtils] Normalized path: " + normalized);
    return normalized;
}

struct ImageData {
    unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    int channels = 0;
};

inline ImageData LoadImageData(const std::string& path, bool alpha) {
    Logger::Info("[FileUtils] Loading image: " + path);
    ImageData img;
    
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        Logger::Error("[FileUtils] Cannot open file: " + path);
        return img;
    }
    auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    Logger::Info("[FileUtils] File size: " + std::to_string(fileSize) + " bytes");
    file.close();
    
    stbi_set_flip_vertically_on_load(false);
    int desired_channels = alpha ? 4 : 3;
    img.data = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, desired_channels);
    if (!img.data) {
        Logger::Error("[FileUtils] stbi_load failed: " + std::string(stbi_failure_reason()));
    } else {
        img.channels = desired_channels;
        Logger::Info("[FileUtils] Loaded image: " + path +
                     " (" + std::to_string(img.width) + "x" + std::to_string(img.height) +
                     ", " + std::to_string(img.channels) + " channels)");
    }
    return img;
}

} // namespace FileUtils