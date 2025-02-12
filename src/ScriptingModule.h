#ifndef SCRIPTINGMODULE_H
#define SCRIPTINGMODULE_H

#include "Module.h"
#include "Logger.h"  // Use Logger for detailed logging instead of std::cout

// ScriptingModule is responsible for integrating a scripting language (e.g., Lua, Python, etc.)
// into the application. It handles initialization, per-frame updates, and shutdown of the scripting engine.
class ScriptingModule : public Module {
public:
    // Initializes the scripting engine.
    // Returns true if the scripting engine was successfully initialized.
    virtual bool Init() override {
        Logger::Info("[ScriptingModule] Initializing scripting engine.");
        // Here, integration of a scripting language (e.g., Lua, Python, etc.) would take place.
        Logger::Debug("[ScriptingModule] Scripting engine integration setup complete.");
        return true;
    }

    // Updates the scripting engine each frame.
    // 'dt' represents the time delta (in seconds) since the last update.
    virtual void Update(float dt) override {
        Logger::Debug("[ScriptingModule] Updating scripting engine (dt = " + std::to_string(dt) + " seconds).");
    }

    // Shuts down the scripting engine and releases any associated resources.
    virtual void Shutdown() override {
        Logger::Info("[ScriptingModule] Shutting down scripting engine.");
    }
};

#endif // SCRIPTINGMODULE_H