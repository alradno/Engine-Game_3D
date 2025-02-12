#ifndef MODULE_H
#define MODULE_H

// The Module class is an abstract interface that defines the lifecycle for a module.
// Each derived module should implement these methods and is encouraged to include
// detailed logging (using the Logger) during initialization, per-frame updates, and shutdown.
class Module {
public:
    virtual ~Module() {}

    // Initializes the module.
    // Returns true if initialization was successful.
    virtual bool Init() = 0;

    // Updates the module each frame.
    // 'dt' represents the elapsed time since the last update (in seconds).
    virtual void Update(float dt) = 0;

    // Releases any resources and shuts down the module.
    virtual void Shutdown() = 0;
};

#endif // MODULE_H
