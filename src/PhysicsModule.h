#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Module.h"
#include "Logger.h"

class PhysicsModule : public Module {
public:
    // Initializes the physics engine (e.g., Bullet, PhysX, etc.).
    virtual bool Init() override {
        Logger::Info("[PhysicsModule] Physics engine initialized.");
        return true;
    }

    // Updates the physics engine.
    // 'dt' represents the time delta (in seconds) since the last update.
    virtual void Update(float dt) override {
        Logger::Debug("[PhysicsModule] Updating physics engine (dt = " + std::to_string(dt) + " seconds).");
    }

    // Shuts down the physics engine and releases allocated resources.
    virtual void Shutdown() override {
        Logger::Info("[PhysicsModule] Physics engine shutdown.");
    }
};

#endif // PHYSICSMODULE_H
