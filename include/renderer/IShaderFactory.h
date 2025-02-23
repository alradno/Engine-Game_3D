/**
 * @file IShaderFactory.h
 * @brief Interface for a shader factory. Provides a method to create Shader objects.
 */

#pragma once
#include <memory>
#include "renderer/Shader.h"

class IShaderFactory {
public:
    virtual std::shared_ptr<Shader> CreateShader(const std::string &vertexPath,
                                                 const std::string &fragmentPath) = 0;
    virtual ~IShaderFactory() = default;
};