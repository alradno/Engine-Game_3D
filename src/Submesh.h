#ifndef SUBMESH_H
#define SUBMESH_H

#include <vector>
#include <iostream>
#include <glad/glad.h>
#include "ModelLoader.h"  // Define la estructura Vertex
#include "Material.h"     // Define la estructura Material
#include "Logger.h"

struct Submesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    Material material;
    
    Submesh() : VAO(0) { }
    
    void setupMesh() {
        if (vertices.empty() || indices.empty()) {
            Logger::Warning("[Submesh::setupMesh] No vertices or indices to setup.");
            return;
        }
        
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &VAO);
        Logger::Debug("[Submesh::setupMesh] Generated VAO ID: " + std::to_string(VAO));
        if (VAO == 0) {
            Logger::Error("[Submesh::setupMesh] VAO generation failed. Ensure an OpenGL context is active.");
        }
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        GLenum err = glGetError();
        if(err != GL_NO_ERROR) {
            Logger::Error("[Submesh::setupMesh] VBO loading error: " + std::to_string(err));
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        err = glGetError();
        if(err != GL_NO_ERROR) {
            Logger::Error("[Submesh::setupMesh] EBO loading error: " + std::to_string(err));
        }
        
        Logger::Info("[Submesh::setupMesh] Configuring submesh with " + std::to_string(vertices.size()) +
                     " vertices and " + std::to_string(indices.size()) + " indices.");
        Logger::Debug("[Submesh::setupMesh] Vertex size: " + std::to_string(sizeof(Vertex)) +
                      " bytes. Offsets - Normal: " + std::to_string(offsetof(Vertex, Normal)) +
                      ", TexCoords: " + std::to_string(offsetof(Vertex, TexCoords)) +
                      ", Tangent: " + std::to_string(offsetof(Vertex, Tangent)));
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);
        
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(3);
        
        glBindVertexArray(0);
    }
    
    void Draw() {
        if (material.albedo) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.albedo->ID);
        }
        if (material.metallicRoughness) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.metallicRoughness->ID);
        }
        if (material.normal) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material.normal->ID);
        }
        
        if (VAO == 0) {
            Logger::Error("[Submesh::Draw] VAO not configured.");
            return;
        }
        if (indices.empty()) {
            Logger::Warning("[Submesh::Draw] Index vector is empty.");
            return;
        }
        Logger::Debug("[Submesh::Draw] Drawing submesh with " + std::to_string(indices.size()) + " indices.");
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            Logger::Error("[Submesh::Draw] OpenGL error: " + std::to_string(err));
        }
        glBindVertexArray(0);
    }
};

#endif
