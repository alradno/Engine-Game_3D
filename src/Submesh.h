#ifndef SUBMESH_H
#define SUBMESH_H

#include <vector>
#include <iostream>
#include <glad/glad.h>
#include "ModelLoader.h"  // Defines the Vertex structure
#include "Material.h"     // Defines the Material structure
#include "Logger.h"

// Structure representing a submesh, which contains vertices, indices, a VAO, and its material.
struct Submesh {
    std::vector<Vertex> vertices;          // Vertex data for the submesh.
    std::vector<unsigned int> indices;     // Index data for drawing the submesh.
    unsigned int VAO;                      // Vertex Array Object ID.
    Material material;                     // Material properties for the submesh.
    
    // Constructor initializes VAO to 0.
    Submesh() : VAO(0) { }
    
    // Sets up the mesh by creating and binding the VAO, VBO, and EBO, and configuring vertex attributes.
    void setupMesh() {
        if (vertices.empty() || indices.empty()) {
            Logger::Warning("[Submesh::setupMesh] No vertices or indices available to set up the mesh.");
            return;
        }
        
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &VAO);
        Logger::Debug("[Submesh::setupMesh] Generated VAO with ID: " + std::to_string(VAO));
        if (VAO == 0) {
            Logger::Error("[Submesh::setupMesh] Failed to generate VAO. Ensure an OpenGL context is active.");
        }
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        // Set up Vertex Buffer Object (VBO)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        GLenum err = glGetError();
        if(err != GL_NO_ERROR) {
            Logger::Error("[Submesh::setupMesh] Error loading VBO: " + std::to_string(err));
        }
        
        // Set up Element Buffer Object (EBO)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        err = glGetError();
        if(err != GL_NO_ERROR) {
            Logger::Error("[Submesh::setupMesh] Error loading EBO: " + std::to_string(err));
        }
        
        Logger::Info("[Submesh::setupMesh] Configuring submesh with " + std::to_string(vertices.size()) +
                     " vertices and " + std::to_string(indices.size()) + " indices.");
        Logger::Debug("[Submesh::setupMesh] Vertex size: " + std::to_string(sizeof(Vertex)) +
                      " bytes. Offsets - Normal: " + std::to_string(offsetof(Vertex, Normal)) +
                      ", TexCoords: " + std::to_string(offsetof(Vertex, TexCoords)) +
                      ", Tangent: " + std::to_string(offsetof(Vertex, Tangent)));
        
        // Configure vertex attributes.
        // Attribute 0: Position (vec3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Attribute 1: Normal (vec3)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        
        // Attribute 2: Texture Coordinates (vec2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);
        
        // Attribute 3: Tangent (vec3)
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(3);
        
        glBindVertexArray(0);
    }
    
    // Draws the submesh by binding the appropriate textures and VAO, then issuing a draw call.
    void Draw() {
        // Bind albedo texture to texture unit 0.
        if (material.albedo) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.albedo->ID);
        }
        // Bind metallic/roughness texture to texture unit 1.
        if (material.metallicRoughness) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.metallicRoughness->ID);
        }
        // Bind normal texture to texture unit 2.
        if (material.normal) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material.normal->ID);
        }
        
        if (VAO == 0) {
            Logger::Error("[Submesh::Draw] VAO is not configured properly.");
            return;
        }
        if (indices.empty()) {
            Logger::Warning("[Submesh::Draw] The indices vector is empty.");
            return;
        }
        Logger::Debug("[Submesh::Draw] Drawing submesh with " + std::to_string(indices.size()) + " indices.");
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            Logger::Error("[Submesh::Draw] OpenGL error during drawing: " + std::to_string(err));
        }
        glBindVertexArray(0);
    }
};

#endif