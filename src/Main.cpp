#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ResourceManager.h"
#include "Model.h"
#include "UniformBuffer.h"   // Se utiliza para otros UBOs
#include "Camera.h"
#include "Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <future>
#include <filesystem>
#include <windows.h>

// Inclusiones para el sistema de escenas
#include "Scene.h"
#include "ModelNode.h"

// Inclusiones para luces (nueva estructura basada en vec4)
#include "Light.h"
#include "LightManager.h"

// Global project root
std::string gProjectRoot;

std::string GetProjectRoot() {
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0) {
        Logger::Error("[GetProjectRoot] Unable to get module file name.");
        return "";
    }
    std::filesystem::path exePath(buffer);
    std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path();
    Logger::Info("[GetProjectRoot] Project root: " + projectRoot.string());
    return projectRoot.string();
}

Camera camera;  // Instancia global de la cámara

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard('W', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard('S', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard('A', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard('D', deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = 1920.0f / 2.0f, lastY = 1080.0f / 2.0f;
    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }
    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);
    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main() {
    // Configuración del Logger
    Logger::SetLogFile("Toxic.log");
    Logger::SetLogLevel(LogLevel::DEBUG);
    
    gProjectRoot = GetProjectRoot();
    if (gProjectRoot.empty()) return -1;
    
    if (!glfwInit()) {
        Logger::Error("[Main] Failed to initialize GLFW.");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Toxic in 3D - PBR", nullptr, nullptr);
    if (!window) {
        Logger::Error("[Main] Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::Error("[Main] Failed to initialize GLAD.");
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // --- Carga de recursos ---
    std::string vertexShaderPath = gProjectRoot + "/shaders/pbr_vertex.glsl";
    std::string fragmentShaderPath = gProjectRoot + "/shaders/pbr_fragment.glsl";
    std::string albedoPath = gProjectRoot + "/assets/car/textures/Material_294_baseColor.png";
    std::string albedo2Path = gProjectRoot + "/assets/car/textures/Material_295_baseColor.png";
    std::string albedo3Path = gProjectRoot + "/assets/car/textures/Material_316_baseColor.png";
    std::string mrPath = gProjectRoot + "/assets/car/textures/Material_294_metallicRoughness.png";
    std::string normalPath = gProjectRoot + "/assets/car/textures/Material_294_normal.png";
    std::string modelPath = gProjectRoot + "/assets/car/scene.gltf";
    
    auto shaderFuture = std::async(std::launch::deferred, [vertexShaderPath, fragmentShaderPath]() {
        return ResourceManager::LoadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), "pbr");
    });
    auto albedoFuture = std::async(std::launch::deferred, [albedoPath]() {
        return ResourceManager::LoadTexture(albedoPath.c_str(), true, "car_albedo");
    });
    auto albedo2Future = std::async(std::launch::deferred, [albedo2Path]() {
        return ResourceManager::LoadTexture(albedo2Path.c_str(), true, "car_albedo2");
    });
    auto albedo3Future = std::async(std::launch::deferred, [albedo3Path]() {
        return ResourceManager::LoadTexture(albedo3Path.c_str(), true, "car_albedo3");
    });
    auto mrFuture = std::async(std::launch::deferred, [mrPath]() {
        return ResourceManager::LoadTexture(mrPath.c_str(), false, "car_mr");
    });
    auto normFuture = std::async(std::launch::deferred, [normalPath]() {
        return ResourceManager::LoadTexture(normalPath.c_str(), false, "car_normal");
    });
    auto modelFuture = std::async(std::launch::deferred, [modelPath]() {
        return ResourceManager::LoadModel(modelPath.c_str(), "car");
    });
    
    auto pbrShader = shaderFuture.get();
    auto albedo = albedoFuture.get();
    auto albedo2 = albedo2Future.get();
    auto albedo3 = albedo3Future.get();
    auto mrMap = mrFuture.get();
    auto normMap = normFuture.get();
    auto carModel = modelFuture.get();
    
    pbrShader->Use();
    glUniform1i(glGetUniformLocation(pbrShader->ID, "albedoMap"), 0);
    glUniform1i(glGetUniformLocation(pbrShader->ID, "metallicRoughnessMap"), 1);
    glUniform1i(glGetUniformLocation(pbrShader->ID, "normalMap"), 2);
    glUniform1i(glGetUniformLocation(pbrShader->ID, "useMaps"), 1);
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(pbrShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // --- Fin carga de recursos ---
    
    // --- Integración del sistema de escenas ---
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    std::shared_ptr<ModelNode> carNode = std::make_shared<ModelNode>(carModel);
    carNode->localTransform = glm::mat4(1.0f);
    carNode->localTransform = glm::translate(carNode->localTransform, glm::vec3(0.0f, -1.0f, 0.0f));
    carNode->localTransform = glm::rotate(carNode->localTransform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    carNode->localTransform = glm::scale(carNode->localTransform, glm::vec3(0.01f));
    scene->GetRoot()->AddChild(carNode);
    // --- Fin integración de escena ---
    
    // --- Configuración de luces usando LightManager ---
    LightManager lightManager;
    
    // Agregar dos luces puntuales (nueva estructura basada en vec4)
    {
        Light pointLight1 = {};
        // Tipo 0 para luz puntual (almacenar el tipo en la componente x)
        pointLight1.typeAndPadding = glm::vec4(0, 0, 0, 0);
        pointLight1.position = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
        pointLight1.direction = glm::vec4(0.0f); // No se utiliza para luz puntual
        // Color rojizo (RGB) y en la componente w se guarda la intensidad (1.0)
        pointLight1.colorAndIntensity = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
        pointLight1.spotParams = glm::vec4(0.0f); // No se usa en punto luz
        lightManager.AddLight(pointLight1);
    
        Light pointLight2 = {};
        pointLight2.typeAndPadding = glm::vec4(0, 0, 0, 0); // Luz puntual
        pointLight2.position = glm::vec4(-5.0f, 5.0f, 5.0f, 1.0f);
        pointLight2.direction = glm::vec4(0.0f);
        // Color blanco con intensidad 1.0
        pointLight2.colorAndIntensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        pointLight2.spotParams = glm::vec4(0.0f);
        lightManager.AddLight(pointLight2);
    }
    
    // Agregar dos luces focales (spot)
    {
        Light spotLight1 = {};
        spotLight1.typeAndPadding = glm::vec4(1, 0, 0, 0); // Tipo 1 para spot
        spotLight1.position = glm::vec4(0.0f, 5.0f, 0.0f, 1.0f);
        // Dirección de la luz, en este caso se normaliza (w no es relevante)
        spotLight1.direction = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        spotLight1.colorAndIntensity = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
        // x: cutOff, y: outerCutOff (en coseno), z y w: padding
        spotLight1.spotParams = glm::vec4(glm::cos(glm::radians(12.5f)),
                                           glm::cos(glm::radians(17.5f)),
                                           0.0f, 0.0f);
        lightManager.AddLight(spotLight1);
    
        Light spotLight2 = {};
        spotLight2.typeAndPadding = glm::vec4(1, 0, 0, 0); // Spot
        spotLight2.position = glm::vec4(0.0f, 5.0f, 5.0f, 1.0f);
        spotLight2.direction = glm::vec4(0.0f, -1.0f, -1.0f, 0.0f);
        spotLight2.colorAndIntensity = glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
        spotLight2.spotParams = glm::vec4(glm::cos(glm::radians(15.0f)),
                                           glm::cos(glm::radians(20.0f)),
                                           0.0f, 0.0f);
        lightManager.AddLight(spotLight2);
    }
    
    // Vincular el bloque uniforme "LightBlock" a la unidad de enlace 1
    unsigned int lightBlockIndex = glGetUniformBlockIndex(pbrShader->ID, "LightBlock");
    if (lightBlockIndex == GL_INVALID_INDEX) {
        Logger::Error("[Main] 'LightBlock' uniform block not found in shader.");
    } else {
        glUniformBlockBinding(pbrShader->ID, lightBlockIndex, 1);
    }
    
    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 view = camera.GetViewMatrix();
        pbrShader->Use();
        glUniformMatrix4fv(glGetUniformLocation(pbrShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(glGetUniformLocation(pbrShader->ID, "camPos"), 1, glm::value_ptr(camera.Position));
        
        // Actualizar el UBO de las luces y vincularlo al binding point 1
        lightManager.UpdateUBO();
        lightManager.lightUBO.BindToPoint(1);
        
        // Actualizar uniformes adicionales (por ejemplo, la iluminación ambiental)
        glm::vec3 ambientLightColor = glm::vec3(0.2f);
        glUniform3fv(glGetUniformLocation(pbrShader->ID, "ambientColor"), 1, glm::value_ptr(ambientLightColor));
        
        // Actualizar la jerarquía de nodos (calcula las transformaciones globales)
        scene->Update();
        // Renderizar la escena: cada nodo envía su matriz "model" y dibuja su contenido.
        scene->Render(*pbrShader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}
