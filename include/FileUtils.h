#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "stb_image.h"
#include "Logger.h"  // Logger is included for detailed logging

namespace FileUtils {

// Normalizes the file path by using "/" as separators and returns the generic string format.
inline std::string NormalizePath(const std::string& path) {
    Logger::Debug("[FileUtils::NormalizePath] Normalizing path: " + path);
    std::filesystem::path fsPath(path);
    std::string normalized = fsPath.generic_string();
    Logger::Debug("[FileUtils::NormalizePath] Normalized path: " + normalized);
    return normalized;
}

// Structure to store image data.
struct ImageData {
    unsigned char* data;
    int width;
    int height;
    int channels;
};

// Loads an image from disk and forces it to a fixed number of channels.
// If 'alpha' is true, forces 4 channels (RGBA); otherwise, 3 channels (RGB).
inline ImageData LoadImageData(const std::string& path, bool alpha) {
    Logger::Info("[FileUtils::LoadImageData] Attempting to load image from: " + path);
    ImageData img;
    
    // Open the file in binary mode and determine its size.
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
    
    // Configure stb_image to not flip the image vertically (adjustable as needed).
    stbi_set_flip_vertically_on_load(false);
    int desired_channels = alpha ? 4 : 3;
    Logger::Debug("[FileUtils::LoadImageData] Desired channels: " + std::to_string(desired_channels));
    
    img.data = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, desired_channels);
    if (!img.data) {
        Logger::Error("[FileUtils::LoadImageData] stbi_load failed for " + path + ". Reason: " + stbi_failure_reason());
    } else {
        // Force the desired number of channels.
        img.channels = desired_channels;
        Logger::Info("[FileUtils::LoadImageData] Successfully loaded image from: " + path +
                     " (Width: " + std::to_string(img.width) +
                     ", Height: " + std::to_string(img.height) +
                     ", Channels: " + std::to_string(img.channels) + ")");
    }
    return img;
}

} // namespace FileUtils

#endif // FILEUTILS_H