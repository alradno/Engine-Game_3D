#ifndef AUDIOMODULE_H
#define AUDIOMODULE_H

#include "Module.h"
#include "Logger.h"  // Use Logger for detailed logging instead of standard output streams

// AudioModule is responsible for managing the audio subsystem.
// It handles initialization, per-frame updates, and proper shutdown of audio resources.
class AudioModule : public Module {
public:
    // Initializes the audio system.
    // Returns true if the initialization is successful.
    virtual bool Init() override {
        Logger::Info("[AudioModule] Starting initialization process for the audio system.");
        // Initialize the audio system (e.g., using libraries like OpenAL, FMOD, etc.)
        Logger::Debug("[AudioModule] Attempting to allocate and initialize audio resources.");

        // Simulate initialization logic.
        bool success = true; // Replace with actual audio initialization code.

        if (success) {
            Logger::Info("[AudioModule] Audio system successfully initialized.");
        } else {
            Logger::Error("[AudioModule] Audio system initialization failed. Check audio device and library configuration.");
        }

        return success;
    }

    // Updates the audio system.
    // 'dt' represents the time delta (in seconds) since the last update call.
    virtual void Update(float dt) override {
        // Log a detailed debug message including the delta time.
        Logger::Debug("[AudioModule] Updating audio system with delta time: " + std::to_string(dt) + " seconds.");
        // Update audio processes such as sound playback, volume adjustments, and real-time effects.
        // Additional debug logs can be inserted here to trace individual audio event updates if needed.
    }

    // Shuts down the audio system and releases any allocated resources.
    virtual void Shutdown() override {
        Logger::Info("[AudioModule] Commencing shutdown sequence for the audio system.");
        // Release audio resources, buffers, and close any open audio devices.
        Logger::Debug("[AudioModule] Releasing allocated audio buffers and cleaning up hardware resources.");
        Logger::Info("[AudioModule] Audio system shutdown complete.");
    }
};

#endif // AUDIOMODULE_H
