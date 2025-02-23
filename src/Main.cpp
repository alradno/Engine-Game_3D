/**
 * @file Main.cpp
 * @brief Entry point for the engine using ECS and YAML-based configuration.
 *
 * This application initializes GLFW and GLAD, loads resources using paths resolved from config.yaml,
 * sets up the ECS (loading entities from entities.yaml), configures a player controller,
 * configures lights from the configuration, and runs the render loop.
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
#include "renderer/ResourceManager.h"
#include "engine/Config.h"
#include "utils/Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include "core/Coordinator.h"
#include "core/EntityLoader.h"
#include "systems/RenderSystem.h"
#include "renderer/Shader.h"
#include "engine/Camera.h"
#include "engine/Light.h"
#include "engine/LightManager.h"
#include "engine/ECSPlayerController.h"
#include "utils/GLDebug.h"
#ifdef _WIN32
#include <windows.h>
#endif

float deltaTime = 0.0f, lastFrame = 0.0f;
Camera camera;

// Callback para errores de GLFW.
void glfwErrorCallback(int error, const char *description)
{
    Logger::Error("[GLFW] Error (" + std::to_string(error) + "): " + std::string(description));
}

// Callback para redimensionar el framebuffer.
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    GLCall(glViewport(0, 0, width, height));
    Logger::ThrottledLog("Main_FramebufferResize", LogLevel::DEBUG,
                         "Framebuffer resized: width = " + std::to_string(width) +
                             ", height = " + std::to_string(height),
                         0.5);
}

void SetWorkingDirectoryToExecutablePath() {
    #ifdef _WIN32
        char exePath[MAX_PATH];
        if (GetModuleFileNameA(NULL, exePath, MAX_PATH) != 0) {
            std::filesystem::path p(exePath);
            // Retroceder al directorio raíz del proyecto, asumiendo que el exe está en build/Release.
            std::filesystem::path projectRoot = p.parent_path().parent_path().parent_path(); // Ajusta según la estructura real
            std::filesystem::current_path(projectRoot);
            Logger::Info(std::string("Working directory set to: ") + std::filesystem::current_path().string());
        } else {
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

        // Cargar configuración desde config.yaml (ruta relativa).
        std::string configPath = "./config/config.yaml";
        Config config = Config::LoadFromFile(configPath);

        // Inyectar la configuración en ResourceManager.
        ResourceManager::SetConfig(config);

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

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::Error("Main: Failed to initialize GLAD.");
            return -1;
        }

        // Configurar callback de depuración de OpenGL.
        SetupOpenGLDebugCallback();

        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glEnable(GL_FRAMEBUFFER_SRGB));
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Inicializar ECS coordinator.
        Coordinator coordinator;
        coordinator.Init();
        coordinator.RegisterComponent<TransformComponent>();
        coordinator.RegisterComponent<RenderComponent>();

        auto renderSystem = coordinator.RegisterSystem<RenderSystem>();
        ECS::Signature renderSignature;
        renderSignature.set(coordinator.GetComponentType<TransformComponent>());
        renderSignature.set(coordinator.GetComponentType<RenderComponent>());
        coordinator.SetSystemSignature<RenderSystem>(renderSignature);

        // Cargar entidades desde entities.yaml (ruta relativa).
        std::string entityConfigPath = "./config/entities.yaml";

        EntityLoader::LoadEntitiesFromYAML(&coordinator, entityConfigPath);

        // Asumir que la primera entidad (ID 0) es el vehículo del jugador.
        ECS::Entity playerEntity = 0;
        ECSPlayerController playerController(&coordinator, playerEntity);

        // Cargar el shader global:
        // Vertex shader se toma de config.vertexShader y fragment shader de config.defaultShader.
        std::string vertexShaderPath = config.vertexShader;
        std::string fragmentShaderPath = config.defaultShader;
        auto defaultShader = ResourceManager::LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), "defaultShader");
        defaultShader->Use();
        GLCall(glUniform1i(glGetUniformLocation(defaultShader->ID, "albedoMap"), 0));
        GLCall(glUniform1i(glGetUniformLocation(defaultShader->ID, "metallicRoughnessMap"), 1));
        GLCall(glUniform1i(glGetUniformLocation(defaultShader->ID, "normalMap"), 2));
        GLCall(glUniform1i(glGetUniformLocation(defaultShader->ID, "useMaps"), 1));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
        GLCall(glUniformMatrix4fv(glGetUniformLocation(defaultShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
        Logger::Info("Main: Default shaders loaded successfully.");

        renderSystem->Init(&coordinator, defaultShader.get(), &camera);

        // Configurar las luces usando la información de config.yaml.
        LightManager lightManager;
        for (const auto &lc : config.lights)
        {
            Light light{};
            if (lc.type == "point")
                light.typeAndPadding = glm::vec4(0, 0, 0, 0);
            light.position = glm::vec4(lc.position, 1.0f);
            light.colorAndIntensity = glm::vec4(lc.color, 1.0f);
            lightManager.AddLight(light);
        }

        unsigned int lightBlockIndex = glGetUniformBlockIndex(defaultShader->ID, "LightBlock");
        if (lightBlockIndex == GL_INVALID_INDEX)
        {
            Logger::Error("Main: 'LightBlock' uniform block not found in shader.");
        }
        else
        {
            GLCall(glUniformBlockBinding(defaultShader->ID, lightBlockIndex, 1));
            Logger::Info("Main: LightBlock bound to binding point 1.");
        }

        Logger::Info("Main: Entering main loop.");
        // Bucle principal de renderizado.
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            playerController.Update(deltaTime);

            // Configurar la cámara fija para observar el modelo.
            camera.Position = glm::vec3(0.0f, 10.0f, 20.0f);
            camera.Front = glm::normalize(glm::vec3(0.0f) - camera.Position);
            camera.Up = glm::vec3(0, 1, 0);

            GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            defaultShader->Use();
            glm::mat4 view = camera.GetViewMatrix();
            GLCall(glUniformMatrix4fv(glGetUniformLocation(defaultShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view)));
            GLCall(glUniform3fv(glGetUniformLocation(defaultShader->ID, "camPos"), 1, glm::value_ptr(camera.Position)));

            lightManager.UpdateUBO();
            lightManager.lightUBO.BindToPoint(1);
            GLCall(glUniform3fv(glGetUniformLocation(defaultShader->ID, "ambientColor"), 1, glm::value_ptr(config.ambientColor)));

            renderSystem->Update(deltaTime);

            // Logging extendido: verificar el estado del framebuffer.
            GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fbStatus != GL_FRAMEBUFFER_COMPLETE)
            {
                Logger::ThrottledLog("Main_FramebufferIncomplete", LogLevel::WARNING,
                                     "[Main] Framebuffer incomplete: " + std::to_string(fbStatus), 0.5);
            }
            else
            {
                Logger::ThrottledLog("Main_FramebufferComplete", LogLevel::DEBUG,
                                     "[Main] Framebuffer complete.", 0.5);
            }

            glfwSwapBuffers(window);
        }

        Logger::Info("Main: Exiting main loop. Cleaning up resources.");
        ResourceManager::Clear();
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
