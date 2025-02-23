// Submesh.h
#pragma once

#include <vector>
#include <glad/glad.h>
#include "core/ModelLoader.h"
#include "renderer/Material.h"
#include "utils/Logger.h"
#include "utils/GLDebug.h"   // Para GLCall, etc.
#include <cstddef>

struct Submesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    Material material;

    // Constructor por defecto
    Submesh() = default;

    // Deshabilitar copia
    Submesh(const Submesh&) = delete;
    Submesh& operator=(const Submesh&) = delete;

    // Constructor de movimiento
    Submesh(Submesh&& other) noexcept {
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        material = std::move(other.material);
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }

    // Asignación por movimiento
    Submesh& operator=(Submesh&& other) noexcept {
        if (this != &other) {
            if(VAO != 0) GLCall(glDeleteVertexArrays(1, &VAO));
            if(VBO != 0) GLCall(glDeleteBuffers(1, &VBO));
            if(EBO != 0) GLCall(glDeleteBuffers(1, &EBO));

            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            material = std::move(other.material);

            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        return *this;
    }

    // Destructor
    ~Submesh(){
        if(VAO != 0) {
            GLCall(glDeleteVertexArrays(1, &VAO));
        }
        if(VBO != 0) {
            GLCall(glDeleteBuffers(1, &VBO));
        }
        if(EBO != 0) {
            GLCall(glDeleteBuffers(1, &EBO));
        }
    }
    
    void setupMesh() {
        if (vertices.empty() || indices.empty()) {
            Logger::Warning("[Submesh] No vertices or indices to setup");
            return;
        }
        
        GLCall(glGenVertexArrays(1, &VAO));
        GLCall(glGenBuffers(1, &VBO));
        GLCall(glGenBuffers(1, &EBO));
        
        GLCall(glBindVertexArray(VAO));
        
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW));
        
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
        
        // Atributos de vértice:
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)));
        GLCall(glEnableVertexAttribArray(1));
        GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)));
        GLCall(glEnableVertexAttribArray(2));
        // Segundo set de UV (ubicación 4)
        GLCall(glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords2)));
        GLCall(glEnableVertexAttribArray(4));
        GLCall(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent)));
        GLCall(glEnableVertexAttribArray(3));
        
        GLCall(glBindVertexArray(0));
        Logger::Info("[Submesh] Setup complete (" + std::to_string(vertices.size()) + " vertices, " +
                     std::to_string(indices.size()) + " indices)");
    }
    
    void Draw() {
        // Optimización: cacheo de binding de texturas para evitar rebinds innecesarios
        static unsigned int lastBoundTex0 = 0;
        static unsigned int lastBoundTex1 = 0;
        static unsigned int lastBoundTex2 = 0;
        
        if (material.albedo) {
            GLCall(glActiveTexture(GL_TEXTURE0));
            if(lastBoundTex0 != material.albedo->ID) {
                GLCall(glBindTexture(GL_TEXTURE_2D, material.albedo->ID));
                lastBoundTex0 = material.albedo->ID;
            }
        }
        if (material.metallicRoughness) {
            GLCall(glActiveTexture(GL_TEXTURE1));
            if(lastBoundTex1 != material.metallicRoughness->ID) {
                GLCall(glBindTexture(GL_TEXTURE_2D, material.metallicRoughness->ID));
                lastBoundTex1 = material.metallicRoughness->ID;
            }
        }
        if (material.normal) {
            GLCall(glActiveTexture(GL_TEXTURE2));
            if(lastBoundTex2 != material.normal->ID) {
                GLCall(glBindTexture(GL_TEXTURE_2D, material.normal->ID));
                lastBoundTex2 = material.normal->ID;
            }
        }
        
        GLCall(glBindVertexArray(VAO));
        GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr));
        GLCall(glBindVertexArray(0));
    }
};