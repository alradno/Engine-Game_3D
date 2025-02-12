#ifndef INPUTMODULE_H
#define INPUTMODULE_H

#include "Module.h"
#include "Logger.h"  // Use Logger for detailed logging instead of standard output streams

class InputModule : public Module {
public:
    // Initializes the input system (keyboard, mouse, gamepad, etc.).
    virtual bool Init() override {
        Logger::Info("[InputModule] Starting initialization of the input system.");
        // Initialize the input system (e.g., keyboard, mouse, gamepad, etc.).
        Logger::Info("[InputModule] Input system successfully initialized.");
        return true;
    }
    
    // Updates the input system.
    // 'dt' represents the time delta (in seconds) since the last update.
    virtual void Update(float dt) override {
        Logger::Debug("[InputModule] Updating input system with delta time: " + std::to_string(dt) + " seconds.");
    }
    
    // Shuts down the input system and releases any allocated resources.
    virtual void Shutdown() override {
        Logger::Info("[InputModule] Shutting down input system.");
    }
};

#endif // INPUTMODULE_H