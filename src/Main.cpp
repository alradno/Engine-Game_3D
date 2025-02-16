#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ResourceManager.h"
#include "ModelLoaderHelper.h"  // Include the helper header
#include "UniformBuffer.h"
#include "Camera.h"
#include "Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <future>
#include <filesystem>
#include <windows.h>
#include "Scene.h"
#include "ModelNode.h"
#include "Light.h"
#include "LightManager.h"
#include "PlayerController.h"
#include "FixedOffsetCameraStrategy.h"

std::string gProjectRoot;

std::string GetProjectRoot() {
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0) {
        Logger::Error("GetProjectRoot: Unable to get module file name.");
        return "";
    }
    std::filesystem::path exePath(buffer);
    std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path();
    Logger::Info("GetProjectRoot: Project root is " + projectRoot.string());
    return projectRoot.string();
}

Camera camera;
float deltaTime = 0.0f, lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Logger::Debug("Framebuffer resized: width = " + std::to_string(width) +
                  ", height = " + std::to_string(height));
}

int main() {
    Logger::SetLogFile("Toxic.log");
    Logger::SetLogLevel(LogLevel::DEBUG);
    Logger::Info("Main: Starting application.");

    gProjectRoot = GetProjectRoot();
    if (gProjectRoot.empty()) {
        Logger::Error("Main: Project root is empty.");
        return -1;
    }

    if (!glfwInit()) {
        Logger::Error("Main: Failed to initialize GLFW.");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Toxic in 3D - PBR", nullptr, nullptr);
    if (!window) {
        Logger::Error("Main: Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::Error("Main: Failed to initialize GLAD.");
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Logger::Info("Main: Loading resources...");
    std::string vertexShaderPath = gProjectRoot + "/shaders/pbr_vertex.glsl";
    std::string fragmentShaderPath = gProjectRoot + "/shaders/pbr_fragment.glsl";

    auto pbrShader = ResourceManager::LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), "pbr");
    pbrShader->Use();
    glUniform1i(glGetUniformLocation(pbrShader->ID, "albedoMap"), 0);
    glUniform1i(glGetUniformLocation(pbrShader->ID, "metallicRoughnessMap"), 1);
    glUniform1i(glGetUniformLocation(pbrShader->ID, "normalMap"), 2);
    glUniform1i(glGetUniformLocation(pbrShader->ID, "useMaps"), 1);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(pbrShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    Logger::Info("Main: Resources loaded successfully.");

    Logger::Info("Main: Setting up scene...");
    auto scene = std::make_shared<Scene>();

    // Load model with transformation using the new helper.
    auto carNode = loadModel(gProjectRoot + "/assets/car/scene.gltf",
                             glm::vec3(0.0f, -1.0f, 0.0f),   // translation
                             glm::vec3(-90.0f, 0.0f, 0.0f),   // rotation in degrees (Euler angles)
                             glm::vec3(0.01f));               // scale
    auto porscheNode = loadModel(gProjectRoot + "/assets/porsche/scene.gltf",
                            glm::vec3(0.0f, -1.0f, 5.0f),   // translation
                            glm::vec3(-90.0f, 0.0f, 0.0f),   // rotation in degrees (Euler angles)
                            glm::vec3(0.01f));
    scene->GetRoot()->AddChild(carNode);
    scene->GetRoot()->AddChild(porscheNode);
    Logger::Info("Main: Scene setup completed.");

    Logger::Info("Main: Creating PlayerController with FixedOffsetCameraStrategy...");
    auto camStrategy = std::make_shared<FixedOffsetCameraStrategy>(glm::vec3(0.0f, 8.0f, 12.0f));
    PlayerController playerController(carNode.get(), &camera, camStrategy);

    Logger::Info("Main: Configuring lights using LightManager...");
    LightManager lightManager;
    {
        Light pointLight1{};
        pointLight1.typeAndPadding = glm::vec4(0, 0, 0, 0);
        pointLight1.position = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
        pointLight1.colorAndIntensity = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
        lightManager.AddLight(pointLight1);

        Light pointLight2{};
        pointLight2.typeAndPadding = glm::vec4(0, 0, 0, 0);
        pointLight2.position = glm::vec4(-5.0f, 5.0f, 5.0f, 1.0f);
        pointLight2.colorAndIntensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        lightManager.AddLight(pointLight2);
    }
    {
        Light spotLight1{};
        spotLight1.typeAndPadding = glm::vec4(1, 0, 0, 0);
        spotLight1.position = glm::vec4(0.0f, 5.0f, 0.0f, 1.0f);
        spotLight1.direction = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        spotLight1.colorAndIntensity = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
        spotLight1.spotParams = glm::vec4(glm::cos(glm::radians(12.5f)),
                                           glm::cos(glm::radians(17.5f)),
                                           0.0f, 0.0f);
        lightManager.AddLight(spotLight1);

        Light spotLight2{};
        spotLight2.typeAndPadding = glm::vec4(1, 0, 0, 0);
        spotLight2.position = glm::vec4(0.0f, 5.0f, 5.0f, 1.0f);
        spotLight2.direction = glm::vec4(0.0f, -1.0f, -1.0f, 0.0f);
        spotLight2.colorAndIntensity = glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
        spotLight2.spotParams = glm::vec4(glm::cos(glm::radians(15.0f)),
                                           glm::cos(glm::radians(20.0f)),
                                           0.0f, 0.0f);
        lightManager.AddLight(spotLight2);
    }

    unsigned int lightBlockIndex = glGetUniformBlockIndex(pbrShader->ID, "LightBlock");
    if (lightBlockIndex == GL_INVALID_INDEX) {
        Logger::Error("Main: 'LightBlock' uniform block not found in shader.");
    } else {
        glUniformBlockBinding(pbrShader->ID, lightBlockIndex, 1);
        Logger::Info("Main: LightBlock bound to binding point 1.");
    }

    Logger::Info("Main: Entering main loop.");
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        playerController.Update(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        pbrShader->Use();
        glUniformMatrix4fv(glGetUniformLocation(pbrShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(glGetUniformLocation(pbrShader->ID, "camPos"), 1, glm::value_ptr(camera.Position));

        lightManager.UpdateUBO();
        lightManager.lightUBO.BindToPoint(1);

        glm::vec3 ambientLightColor(0.2f);
        glUniform3fv(glGetUniformLocation(pbrShader->ID, "ambientColor"), 1, glm::value_ptr(ambientLightColor));

        scene->Update();
        scene->Render(*pbrShader);

        glfwSwapBuffers(window);
    }

    Logger::Info("Main: Exiting main loop. Cleaning up resources.");
    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}