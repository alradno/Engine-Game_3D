/**
 * @file ShaderFactory.h
 * @brief Concrete implementation of IShaderFactory. Creates Shader objects.
 */

 #pragma once
 #include "IShaderFactory.h"
 #include "Logger.h"
 #include <memory>
 #include <string>
 
 class ShaderFactory : public IShaderFactory {
 public:
     std::shared_ptr<Shader> CreateShader(const std::string& vertexPath,
                                          const std::string& fragmentPath) override {
         auto shader = std::make_shared<Shader>();
         shader->Compile(vertexPath.c_str(), fragmentPath.c_str());
         Logger::Info("[ShaderFactory] Shader created with ID: " + std::to_string(shader->ID));
         return shader;
     }
 }; 