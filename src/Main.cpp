/**
 * @file Main.cpp
 * @brief Entry point for the medical graphics engine using ECS and YAML.
 *
 * This application initializes GLFW and GLAD, loads shader and resource data,
 * sets up the ECS (loading entities from a YAML configuration file),
 * configures a player controller to move the player entity with WASD and arrow keys,
 * configures lights, and runs the render loop.
 */

 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include <iostream>
 #include "ResourceManager.h"
 #include "Logger.h"
 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <glm/gtc/type_ptr.hpp>
 #include <filesystem>
 #include <windows.h>
 #include "Coordinator.h"
 #include "EntityLoader.h"
 #include "RenderSystem.h"
 #include "Shader.h"
 #include "Camera.h"
 #include "Light.h"
 #include "LightManager.h"
 #include "ECSPlayerController.h"  // New player controller based on ECS
 
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
     
     GLFWwindow* window = glfwCreateWindow(1920, 1080, "Toxic - 3D", nullptr, nullptr);
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
     
     // Initialize ECS coordinator
     Coordinator coordinator;
     coordinator.Init();
     coordinator.RegisterComponent<TransformComponent>();
     coordinator.RegisterComponent<RenderComponent>();
     
     // Register RenderSystem and set its signature (requires TransformComponent and RenderComponent)
     auto renderSystem = coordinator.RegisterSystem<RenderSystem>();
     ECS::Signature renderSignature;
     renderSignature.set(coordinator.GetComponentType<TransformComponent>());
     renderSignature.set(coordinator.GetComponentType<RenderComponent>());
     coordinator.SetSystemSignature<RenderSystem>(renderSignature);
     
     // Load entities from YAML configuration (ensure that config/entities.yaml exists)
     std::string entityConfigPath = gProjectRoot + "/config/entities.yaml";
     EntityLoader::LoadEntitiesFromYAML(&coordinator, entityConfigPath);
     
     // Asumimos que la primera entidad (ID 0) es el vehículo del jugador.
     ECS::Entity playerEntity = 0;
     ECSPlayerController playerController(&coordinator, playerEntity);
     
     // Load shader resources
     std::string vertexShaderPath = gProjectRoot + "/shaders/pbr_vertex.glsl";
     std::string fragmentShaderPath = gProjectRoot + "/shaders/pbr_fragment.glsl";
     auto pbrShader = ResourceManager::LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), "pbr");
     pbrShader->Use();
     glUniform1i(glGetUniformLocation(pbrShader->ID, "albedoMap"), 0);
     glUniform1i(glGetUniformLocation(pbrShader->ID, "metallicRoughnessMap"), 1);
     glUniform1i(glGetUniformLocation(pbrShader->ID, "normalMap"), 2);
     glUniform1i(glGetUniformLocation(pbrShader->ID, "useMaps"), 1);
     glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 100.0f);
     glUniformMatrix4fv(glGetUniformLocation(pbrShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
     Logger::Info("Main: Resources loaded successfully.");
     
     // Initialize the RenderSystem with its dependencies (coordinator, shader, and camera)
     renderSystem->Init(&coordinator, pbrShader.get(), &camera);
     
     // Configure lights using LightManager (se mantiene la funcionalidad original)
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
         
         // Update player controller (WASD movement, rotation with arrow keys)
         playerController.Update(deltaTime);
         
         // --- Actualización de la cámara: posición fija en el mundo ---
         // La cámara se fija en (0,10,20) mirando al origen para observar el modelo sin seguirlo.
         camera.Position = glm::vec3(0.0f, 10.0f, 20.0f);
         camera.Front = glm::normalize(glm::vec3(0.0f) - camera.Position);
         camera.Up = glm::vec3(0, 1, 0);
         
         glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         
         // Update shader uniforms for camera and view BEFORE rendering
         pbrShader->Use();
         glm::mat4 view = camera.GetViewMatrix();
         glUniformMatrix4fv(glGetUniformLocation(pbrShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
         glUniform3fv(glGetUniformLocation(pbrShader->ID, "camPos"), 1, glm::value_ptr(camera.Position));
         
         lightManager.UpdateUBO();
         lightManager.lightUBO.BindToPoint(1);
         
         glm::vec3 ambientLightColor(0.2f);
         glUniform3fv(glGetUniformLocation(pbrShader->ID, "ambientColor"), 1, glm::value_ptr(ambientLightColor));
         
         // Render all entities via the RenderSystem (which iterates over entities with Transform and Render components)
         renderSystem->Update(deltaTime);
         
         glfwSwapBuffers(window);
     }
     
     Logger::Info("Main: Exiting main loop. Cleaning up resources.");
     ResourceManager::Clear();
     glfwTerminate();
     return 0;
 } 