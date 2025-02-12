#ifndef RENDERERMODULE_H
#define RENDERERMODULE_H

#include "Module.h"
#include "Logger.h"  // Use Logger for detailed logging instead of std::cout
#include <glm/glm.hpp>

// RendererModule is responsible for initializing and managing the rendering system.
// This includes creating the window, setting up the OpenGL context, and rendering the scene.
class RendererModule : public Module {
public:
    // Initializes the renderer.
    // Returns true if the initialization was successful.
    virtual bool Init() override {
        Logger::Info("[RendererModule] Initializing renderer.");
        // Here, the window and OpenGL context would be created and configured.
        Logger::Debug("[RendererModule] Creating window and OpenGL context...");
        // Additional initialization code goes here.
        Logger::Info("[RendererModule] Renderer initialized successfully.");
        return true;
    }
    
    // Updates the renderer each frame.
    // 'dt' represents the time elapsed since the last update (in seconds).
    virtual void Update(float dt) override {
        Logger::Debug("[RendererModule] Rendering frame (dt = " + std::to_string(dt) + " seconds).");
        // Scene rendering would occur here.
    }
    
    // Shuts down the renderer and releases any associated resources.
    virtual void Shutdown() override {
        Logger::Info("[RendererModule] Shutting down renderer.");
        // Cleanup code for the window, OpenGL context, and other resources goes here.
    }
};

#endif // RENDERERMODULE_H