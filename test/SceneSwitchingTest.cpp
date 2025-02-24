/**
 * @file SceneSwitchingTest.cpp
 * @brief Test que simula el ciclo de ejecución del motor,
 * alternando entre Scene1 y Scene2 100 veces y registrando el uso de memoria.
 */

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Evitar que Windows defina las macros min y max.
#define NOMINMAX

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <limits>

#include "engine/Config.h"
#include "utils/Logger.h"
#include "utils/GLDebug.h"
#include "renderer/ResourceManager.h"
#include "engine/SceneManager.h"
#include "../scenes/Scene1.h"
#include "../scenes/Scene2.h"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
size_t getCurrentMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return pmc.WorkingSetSize; // en bytes
    }
    return 0;
}
#else
#include <unistd.h>
#include <sys/resource.h>
size_t getCurrentMemoryUsage()
{
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    return r_usage.ru_maxrss * 1024; // ru_maxrss en kilobytes
}
#endif

// Umbral (solo informativo)
const size_t MEMORY_THRESHOLD = 1024 * 1024; // 1 MB

// Estructura para almacenar registros de memoria
struct MemoryRecord
{
    int iteration;
    std::string sceneName;
    size_t memoryBytes;
};

// Callback de error de GLFW
void glfwErrorCallback(int error, const char *description)
{
    Logger::Error("[GLFW] Error (" + std::to_string(error) + "): " + std::string(description));
}

// Callback para redimensionar el framebuffer
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    GLCall(glViewport(0, 0, width, height));
    Logger::ThrottledLog("Test_FramebufferResize", LogLevel::DEBUG,
                         "Framebuffer resized: width = " + std::to_string(width) +
                             ", height = " + std::to_string(height),
                         0.5);
}

// Cambiar el directorio de trabajo al path del ejecutable (Windows)
void SetWorkingDirectoryToExecutablePath()
{
#ifdef _WIN32
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) != 0)
    {
        std::filesystem::path p(exePath);
        // Ajusta según la estructura de tu proyecto (por ejemplo, subiendo 4 niveles)
        std::filesystem::path projectRoot = p.parent_path().parent_path().parent_path().parent_path();
        std::filesystem::current_path(projectRoot);
        Logger::Info("Working directory set to: " + std::filesystem::current_path().string());
    }
    else
    {
        Logger::Error("Error obtaining the executable path.");
    }
#endif
}

// Función auxiliar de renderizado: durante 'delaySeconds', se limpia el framebuffer, se actualiza y se renderiza la escena.
void renderForSeconds(GLFWwindow *window, float delaySeconds)
{
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count() < delaySeconds)
    {
        GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // Simular un deltaTime fijo (~60 fps)
        SceneManager::GetInstance().Update(0.016f);
        SceneManager::GetInstance().Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main()
{
    try
    {
        glfwSetErrorCallback(glfwErrorCallback);
        Logger::SetLogFile("ToxicTest.log");
        Logger::SetLogLevel(LogLevel::DEBUG);
        Logger::Info("Test: Starting application.");
        SetWorkingDirectoryToExecutablePath();

        // Cargar configuración global
        std::string configPath = "../config/config.yaml";
        Config config = Config::LoadFromFile(configPath);
        ResourceManager::SetConfig(config);

        // Inicializar GLFW.
        if (!glfwInit())
        {
            Logger::Error("Test: Failed to initialize GLFW.");
            return -1;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        // Crear una ventana de 1920x1080.
        GLFWwindow *window = glfwCreateWindow(1920, 1080, "ToxicTest - Scene Switching", nullptr, nullptr);
        if (!window)
        {
            Logger::Error("Test: Failed to create GLFW window.");
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Inicializar GLAD.
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::Error("Test: Failed to initialize GLAD.");
            glfwTerminate();
            return -1;
        }

        SetupOpenGLDebugCallback();
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glEnable(GL_FRAMEBUFFER_SRGB));
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Inicializar el SceneManager con la primera escena (Scene1).
        SceneManager::GetInstance().SwitchScene(std::make_unique<Scene1>());
        // Renderizar durante 10 segundos para estabilizar Scene1.
        renderForSeconds(window, 1.0f);
        std::vector<MemoryRecord> records;
        size_t mem = getCurrentMemoryUsage();
        records.push_back({1, "Scene1", mem});
        std::cout << "Iteration 1: Scene1 memory = " << mem / (1024.0 * 1024.0) << " MB" << std::endl;

        // Cambiar a Scene2 y renderizar durante 10 segundos.
        std::cout << "Iteration 2: Switching to Scene2" << std::endl;
        SceneManager::GetInstance().SwitchScene(std::make_unique<Scene2>());
        renderForSeconds(window, 2.0f);
        mem = getCurrentMemoryUsage();
        records.push_back({2, "Scene2", mem});
        std::cout << "Iteration 2: Scene2 memory = " << mem / (1024.0 * 1024.0) << " MB" << std::endl;

        // Alternar entre escenas 98 veces para un total de 100 iteraciones.
        for (int i = 3; i <= 100; ++i)
        {
            if (i % 2 == 0)
            {
                std::cout << "Iteration " << i << ": Switching to Scene2" << std::endl;
                SceneManager::GetInstance().SwitchScene(std::make_unique<Scene2>());
            }
            else
            {
                std::cout << "Iteration " << i << ": Switching to Scene1" << std::endl;
                SceneManager::GetInstance().SwitchScene(std::make_unique<Scene1>());
            }
            // Renderizar durante 0.1 segundo para estabilizar la escena actual.
            renderForSeconds(window, 0.1f);
            mem = getCurrentMemoryUsage();
            std::string sceneName = (i % 2 == 0) ? "Scene2" : "Scene1";
            records.push_back({i, sceneName, mem});
            std::cout << "Iteration " << i << ": " << sceneName
                      << " memory = " << mem / (1024.0 * 1024.0) << " MB" << std::endl;

            // Si se presiona ESC, salir inmediatamente
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                std::cout << "ESC pressed. Exiting loop..." << std::endl;
                glfwSetWindowShouldClose(window, true);
                break;
            }
        }

        // Imprimir la tabla completa de registros de memoria.
        std::cout << "\n--- Memory Usage Table (Iteration | Scene | Memory MB) ---\n";
        std::cout << std::setw(10) << "Iteration"
                  << std::setw(10) << "Scene"
                  << std::setw(15) << "Memory (MB)" << std::endl;
        for (const auto &rec : records)
        {
            std::cout << std::setw(10) << rec.iteration
                      << std::setw(10) << rec.sceneName
                      << std::setw(15) << rec.memoryBytes / (1024.0 * 1024.0) << std::endl;
        }

        std::cout << "\nPress ESC to close the test..." << std::endl;
        // Bucle final para esperar a que el usuario presione ESC.
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    catch (const std::exception &e)
    {
        Logger::Error(std::string("[Test] Exception caught: ") + e.what());
        std::cerr << "An unexpected error occurred. Check the log." << std::endl;
        glfwTerminate();
        return -1;
    }
    catch (...)
    {
        Logger::Error("[Test] Unknown exception caught.");
        std::cerr << "An unknown error occurred. Check the log." << std::endl;
        glfwTerminate();
        return -1;
    }
}