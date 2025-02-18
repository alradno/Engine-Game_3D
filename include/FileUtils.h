#pragma once
#include <string>
#include <filesystem>
#include "Logger.h"

namespace FileUtils
{

    inline std::string NormalizePath(const std::string &path)
    {
        std::filesystem::path p(path);
        std::string normalized = p.generic_string();
        Logger::Debug("[FileUtils] Normalized path: " + normalized);
        return normalized;
    }

    inline std::string ResolvePath(const std::string &base, const std::string &relative)
    {
        std::string rel = relative;
        if (!rel.empty() && rel.front() == '/')
            rel.erase(0, 1);
        std::filesystem::path basePath(base);
        std::filesystem::path fullPath = basePath / rel;
        std::string resolved = fullPath.generic_string();
        Logger::Debug("[FileUtils] Resolved path: " + resolved);
        return resolved;
    }

    // Estructura para la carga de imágenes.
    struct ImageData
    {
        unsigned char *data = nullptr;
        int width = 0;
        int height = 0;
        int channels = 0;
    };

} // namespace FileUtils
