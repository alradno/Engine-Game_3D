#include "MenuScene.h"
#include "core/EntityLoader.h"
#include "systems/RenderSystem.h"
#include "renderer/ResourceManager.h"
#include "utils/Logger.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"
#include "core/InputManager.h"
#include "engine/SceneManager.h"
#include "Scene1.h"  // Escena de juego

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <cstdlib>  // Para exit()

MenuScene::MenuScene() 
    : renderSystem(nullptr), uiVAO(0), uiVBO(0), uiEBO(0), whiteTexture(0), textRenderer(nullptr)
{ }

MenuScene::~MenuScene() {
    Destroy();
}

void MenuScene::Init() {
    Logger::Info("[MenuScene] Inicializando escena de menú");

    // Inicializar el coordinator.
    coordinator = std::make_unique<Coordinator>();
    coordinator->Init();
    coordinator->RegisterComponent<TransformComponent>();
    coordinator->RegisterComponent<TextComponent>();
    coordinator->RegisterComponent<ButtonComponent>();

    // Registrar el sistema de renderizado para UI (Transform y Text).
    renderSystem = coordinator->RegisterSystem<RenderSystem>();
    ECS::Signature signature;
    signature.set(coordinator->GetComponentType<TransformComponent>());
    signature.set(coordinator->GetComponentType<TextComponent>());
    coordinator->SetSystemSignature<RenderSystem>(signature);

    // Cargar shader para UI (para dibujar el quad del botón).
    shader = sceneResources.LoadShader("ui_vertex.glsl", "ui_fragment.glsl", "menuShader");
    if (!shader) {
        Logger::Error("[MenuScene] Error al cargar shader 'menuShader'.");
        return;
    }

    // --- Creación de un quad para el botón ---
    float vertices[] = {
        // Posiciones         // Coordenadas de textura
        -0.5f, -0.5f,         0.0f, 0.0f,
         0.5f, -0.5f,         1.0f, 0.0f,
         0.5f,  0.5f,         1.0f, 1.0f,
        -0.5f,  0.5f,         0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);
    glGenBuffers(1, &uiEBO);

    glBindVertexArray(uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- Crear una textura blanca de 1x1 ---
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    unsigned char whitePixel[4] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    Logger::Debug("[MenuScene] whiteTexture ID: " + std::to_string(whiteTexture));

    // Cargar las entidades definidas en el YAML del menú.
    EntityLoader::LoadEntitiesFromYAML(coordinator.get(), "./config/entities_menu.yaml");
    // Si no se cargaron entidades, crear una de prueba.
    auto textEntities = coordinator->GetEntitiesWithComponent<TextComponent>();
    if (textEntities.empty()) {
        Logger::Warning("[MenuScene] No se encontraron entidades con TextComponent. Creando entidad de prueba.");
        ECS::Entity testEntity = coordinator->CreateEntity();
        TransformComponent testTransform;
        testTransform.translation = glm::vec3(-0.5f, 0.3f, 0.0f);
        testTransform.scale = glm::vec3(0.3f, 0.1f, 1.0f);
        coordinator->AddComponent<TransformComponent>(testEntity, testTransform);
        // Usar color negro para el texto para que sea visible sobre fondo blanco.
        TextComponent testText("Play", "fonts/american_captain/American Captain.otf", 32, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        coordinator->AddComponent<TextComponent>(testEntity, testText);
        ButtonComponent testButton("play");
        coordinator->AddComponent<ButtonComponent>(testEntity, testButton);
        Logger::Info("[MenuScene] Entidad de prueba creada: " + std::to_string(testEntity));
    }

    // --- Inicializar el TextRenderer (resolución 1920x1080) ---
    textRenderer = new TextRenderer(1920, 1080, &sceneResources);
    textRenderer->Load("fonts/american_captain/American Captain.otf", 32);

    Logger::Info("[MenuScene] Escena de menú inicializada.");
}

void MenuScene::Update(float dt) {
    // Procesar clicks en los botones.
    if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 mousePos = InputManager::GetMousePosition();
        auto buttonEntities = coordinator->GetEntitiesWithComponent<ButtonComponent>();
        for (auto entity : buttonEntities) {
            auto& transform = coordinator->GetComponent<TransformComponent>(entity);
            auto& button = coordinator->GetComponent<ButtonComponent>(entity);
            glm::vec2 pos(transform.translation.x, transform.translation.y);
            glm::vec2 halfSize(transform.scale.x * 0.5f, transform.scale.y * 0.5f);
            if (mousePos.x >= pos.x - halfSize.x && mousePos.x <= pos.x + halfSize.x &&
                mousePos.y >= pos.y - halfSize.y && mousePos.y <= pos.y + halfSize.y) {
                Logger::Info("[MenuScene] Botón presionado: " + button.action);
                if (button.action == "play") {
                    SceneManager::GetInstance().SwitchScene(std::make_unique<Scene1>());
                    return;  // Salir inmediatamente para evitar procesar más en la escena actual.
                } else if (button.action == "settings") {
                    Logger::Info("[MenuScene] Botón Settings presionado. Funcionalidad por implementar.");
                } else if (button.action == "exit") {
                    Logger::Info("[MenuScene] Botón Exit presionado. Saliendo de la aplicación.");
                    exit(0);
                }
            }
        }
    }
}

void MenuScene::Render() {
    // Primero, dibujar el quad del botón (fondo blanco) para cada entidad.
    if (shader) {
        shader->Use();
        glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(ortho));
        auto textEntities = coordinator->GetEntitiesWithComponent<TextComponent>();
        for (auto entity : textEntities) {
            auto& transform = coordinator->GetComponent<TransformComponent>(entity);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(transform.translation.x, transform.translation.y, transform.translation.z));
            model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform4fv(glGetUniformLocation(shader->ID, "color"), 1, glm::value_ptr(glm::vec4(1.0f)));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, whiteTexture);
            glUniform1i(glGetUniformLocation(shader->ID, "uiTexture"), 0);
            glBindVertexArray(uiVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    // Para renderizar el texto, desactivamos el depth test para que se dibuje sobre la UI.
    glDisable(GL_DEPTH_TEST);
    // Renderizar el texto de cada entidad usando el TextRenderer.
    auto textEntities = coordinator->GetEntitiesWithComponent<TextComponent>();
    for (auto entity : textEntities) {
        auto& transform = coordinator->GetComponent<TransformComponent>(entity);
        auto& text = coordinator->GetComponent<TextComponent>(entity);
        // Convertir coordenadas normalizadas a píxeles (suponiendo 1920x1080).
        float x_pixel = (transform.translation.x + 1.0f) * 1920.0f * 0.5f;
        float y_pixel = (transform.translation.y + 1.0f) * 1080.0f * 0.5f;
        glm::vec3 textColor = glm::vec3(text.color.r, text.color.g, text.color.b);
        textRenderer->RenderText(text.content, x_pixel, y_pixel, 1.0f, textColor);
    }
    // Rehabilitar depth test si es necesario para otras partes de la escena.
    glEnable(GL_DEPTH_TEST);
}

void MenuScene::Destroy() {
    Logger::Info("[MenuScene] Destruyendo escena de menú");
    if (coordinator) {
        coordinator->Clear();
        coordinator.reset();
    }
    sceneResources.Clear();
    if (uiVAO) glDeleteVertexArrays(1, &uiVAO);
    if (uiVBO) glDeleteBuffers(1, &uiVBO);
    if (uiEBO) glDeleteBuffers(1, &uiEBO);
    if (whiteTexture) glDeleteTextures(1, &whiteTexture);
    if (textRenderer) {
        delete textRenderer;
        textRenderer = nullptr;
    }
    Logger::Info("[MenuScene] Escena de menú destruida.");
}