#include "Engine.h"
#include "Logger.h"  // Use Logger for detailed logging instead of iostream

Engine::Engine() : m_Running(false) {
    Logger::Info("[Engine] Engine instance created.");
}

Engine::~Engine() {
    Logger::Info("[Engine] Engine destructor called. Initiating shutdown sequence.");
    Shutdown();
}

// Adds a new module to the engine's module list.
void Engine::AddModule(std::shared_ptr<Module> module) {
    m_Modules.push_back(module);
    Logger::Debug("[Engine] Module added to the engine.");
}

// Initializes all modules. Returns true if all modules are successfully initialized.
bool Engine::Init() {
    Logger::Info("[Engine] Starting initialization of engine modules.");
    for (auto &module : m_Modules) {
        if (!module->Init()) {
            Logger::Error("[Engine] A module failed to initialize. Aborting engine initialization.");
            return false;
        }
    }
    m_Running = true;
    Logger::Info("[Engine] All modules initialized successfully. Engine is now running.");
    return true;
}

// Runs the main engine loop. For this example, a fixed delta time (dt) is used (~60 FPS).
void Engine::Run() {
    float dt = 0.016f; // Fixed delta time for ~60 FPS.
    Logger::Info("[Engine] Entering main loop with fixed dt = " + std::to_string(dt) + " seconds.");
    while (m_Running) {
        // Update all modules with the fixed delta time.
        for (auto &module : m_Modules) {
            module->Update(dt);
            Logger::Debug("[Engine] Updated a module with dt = " + std::to_string(dt) + " seconds.");
        }
        // In a real engine, events would be processed and time updated here.
        // For this example, we exit the loop after one iteration.
        m_Running = false;
        Logger::Info("[Engine] Main loop iteration complete. Exiting main loop.");
    }
}

// Shuts down all modules and clears the module list.
void Engine::Shutdown() {
    Logger::Info("[Engine] Shutting down engine modules.");
    for (auto &module : m_Modules) {
        module->Shutdown();
        Logger::Debug("[Engine] A module has been shut down successfully.");
    }
    m_Modules.clear();
    m_Running = false;
    Logger::Info("[Engine] Engine shutdown complete. All modules have been cleared.");
}