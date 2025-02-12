#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <memory>
#include "Module.h"
#include "Logger.h"  // Include Logger to provide detailed logging messages

// The Engine class is responsible for managing the lifecycle of various modules.
// It supports initializing, running, and shutting down the modules.
class Engine {
public:
    // Constructor: Creates an instance of the engine.
    Engine();
    // Destructor: Ensures that the engine shuts down properly.
    ~Engine();

    // Initializes all added modules.
    // Returns true if all modules are successfully initialized.
    bool Init();
    // Runs the main loop by updating each module.
    void Run();
    // Shuts down and releases all modules.
    void Shutdown();

    // Adds a module to the engine.
    void AddModule(std::shared_ptr<Module> module);

    // Retrieves a module of a specific type.
    // This method searches for a module that can be cast to type T.
    template<typename T>
    std::shared_ptr<T> GetModule();
    
private:
    bool m_Running;  // Indicates whether the engine is currently running.
    std::vector<std::shared_ptr<Module>> m_Modules;  // List of modules managed by the engine.
};

// Template method to retrieve a module of a specific type T.
// It logs the search process and returns the first matching module.
template<typename T>
std::shared_ptr<T> Engine::GetModule() {
    Logger::Debug("[Engine::GetModule] Attempting to retrieve module of the specified type.");
    for (auto &module : m_Modules) {
        auto casted = std::dynamic_pointer_cast<T>(module);
        if (casted) {
            Logger::Debug("[Engine::GetModule] Module found and successfully casted to the required type.");
            return casted;
        }
    }
    Logger::Warning("[Engine::GetModule] No module of the specified type was found.");
    return nullptr;
}

#endif // ENGINE_H
