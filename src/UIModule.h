#ifndef UIMODULE_H
#define UIMODULE_H

#include "Module.h"
#include "Logger.h"  // Use Logger for detailed logging

// UIModule is responsible for managing the user interface system (menus, HUD, etc.).
class UIModule : public Module {
public:
    // Initializes the UI system.
    // Returns true if the initialization was successful.
    virtual bool Init() override {
        Logger::Info("[UIModule] UI system initialized.");
        // Initialize the user interface system (e.g., menus, HUD, etc.).
        return true;
    }
    
    // Updates the UI system.
    // 'dt' represents the time delta (in seconds) since the last update.
    virtual void Update(float dt) override {
        Logger::Debug("[UIModule] Updating UI (dt = " + std::to_string(dt) + " seconds).");
    }
    
    // Shuts down the UI system and releases any associated resources.
    virtual void Shutdown() override {
        Logger::Info("[UIModule] UI system shutdown.");
    }
};

#endif // UIMODULE_H