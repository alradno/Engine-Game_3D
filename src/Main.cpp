/**
 * @file Main.cpp
 * @brief Entry point for the engine using ECS and YAML-based configuration.
 *
 * This application initializes GLFW and GLAD, loads configuration from config.yaml,
 * sets up the scene management system, and runs the render loop.
 */

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include "engine/Config.h"
#include "utils/Logger.h"
#include "utils/GLDebug.h"
#include "renderer/ResourceManager.h"
#include "engine/SceneManager.h"
#include "../scenes/Scene1.h"
#include "../scenes/Scene2.h"

#ifdef _WIN32
#include <windows.h>
#endif

// Global variables for timing
float deltaTime = 0.0f, lastFrame = 0.0f;

// Callback for GLFW errors.
void glfwErrorCallback(int error, const char *description)
{
    Logger::Error("[GLFW] Error (" + std::to_string(error) + "): " + std::string(description));
}

// Callback for resizing the framebuffer.
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    GLCall(glViewport(0, 0, width, height));
    Logger::ThrottledLog("Main_FramebufferResize", LogLevel::DEBUG,
                         "Framebuffer resized: width = " + std::to_string(width) +
                             ", height = " + std::to_string(height),
                         0.5);
}

// Set working directory to the executable path (Windows version).
void SetWorkingDirectoryToExecutablePath()
{
#ifdef _WIN32
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) != 0)
    {
        std::filesystem::path p(exePath);
        // Adjust to project root assuming the exe is in build/Release (modify as needed)
        std::filesystem::path projectRoot = p.parent_path().parent_path().parent_path();
        std::filesystem::current_path(projectRoot);
        Logger::Info("Working directory set to: " + std::filesystem::current_path().string());
    }
    else
    {
        Logger::Error("Error obtaining the executable path.");
    }
#endif
}

int main()
{
    try
    {
        glfwSetErrorCallback(glfwErrorCallback);
        Logger::SetLogFile("Toxic.log");
        Logger::SetLogLevel(LogLevel::DEBUG);
        Logger::Info("Main: Starting application.");
        SetWorkingDirectoryToExecutablePath();

        // Load global configuration from config.yaml
        std::string configPath = "./config/config.yaml";
        Config config = Config::LoadFromFile(configPath);
        ResourceManager::SetConfig(config);

        // Initialize GLFW.
        if (!glfwInit())
        {
            Logger::Error("Main: Failed to initialize GLFW.");
            return -1;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        GLFWwindow *window = glfwCreateWindow(1920, 1080, "Toxic - 3D", nullptr, nullptr);
        if (!window)
        {
            Logger::Error("Main: Failed to create GLFW window.");
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Initialize GLAD.
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::Error("Main: Failed to initialize GLAD.");
            return -1;
        }

        // Setup OpenGL debug callback.
        SetupOpenGLDebugCallback();

        // Enable OpenGL features.
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glEnable(GL_FRAMEBUFFER_SRGB));
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Initialize the SceneManager with the initial scene (Scene1).
        SceneManager::GetInstance().SwitchScene(std::make_unique<Scene1>());

        Logger::Info("Main: Entering main loop.");
        // Main render loop.
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            
            // Limpiar el framebuffer:
            GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            // Switch scenes: Press key '2' to switch to Scene2, key '1' to switch back to Scene1.
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            {
                SceneManager::GetInstance().SwitchScene(std::make_unique<Scene2>());
            }
            else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            {
                SceneManager::GetInstance().SwitchScene(std::make_unique<Scene1>());
            }

            // Update and render the current scene.
            SceneManager::GetInstance().Update(deltaTime);
            SceneManager::GetInstance().Render();

            // Verify framebuffer status.
            GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fbStatus != GL_FRAMEBUFFER_COMPLETE)
            {
                Logger::ThrottledLog("Main_FramebufferIncomplete", LogLevel::WARNING,
                                     "[Main] Framebuffer incomplete: " + std::to_string(fbStatus), 5.0);
            }
            else
            {
                Logger::ThrottledLog("Main_FramebufferComplete", LogLevel::DEBUG,
                                     "[Main] Framebuffer complete.", 5.0);
            }

            glfwSwapBuffers(window);
        }

        Logger::Info("Main: Exiting main loop. Cleaning up resources.");
        glfwTerminate();
        return 0;
    }
    catch (const std::exception &e)
    {
        Logger::Error(std::string("[Main] Exception caught in main loop: ") + e.what());
        std::cerr << "Ocurrió un error inesperado. Por favor, revisa el log para más detalles." << std::endl;
        glfwTerminate();
        return -1;
    }
    catch (...)
    {
        Logger::Error("[Main] Unknown exception caught in main loop.");
        std::cerr << "Ocurrió un error desconocido. Por favor, revisa el log." << std::endl;
        glfwTerminate();
        return -1;
    }
}
