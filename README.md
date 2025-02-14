# Toxic 3D Engine

Toxic is a modular 3D engine designed for real-time rendering using OpenGL. The engine leverages a component-based architecture where each subsystem (audio, input, physics, rendering, resource management, scene graph, scripting, and UI) is implemented as a separate module. This design promotes scalability, clean separation of concerns, and ease of extension.

---

## Table of Contents

- [Overview](#overview)
- [Directory Structure and File Descriptions](#directory-structure-and-file-descriptions)
  - [Core Engine and Module Management](#core-engine-and-module-management)
  - [Logging and File Utilities](#logging-and-file-utilities)
  - [Resource Management](#resource-management)
  - [Rendering Components](#rendering-components)
  - [Model and Mesh Loading](#model-and-mesh-loading)
  - [Scene Graph and Nodes](#scene-graph-and-nodes)
  - [Camera and Player Control](#camera-and-player-control)
  - [Subsystem Modules](#subsystem-modules)
  - [Shaders and External Assets](#shaders-and-external-assets)
- [Dependencies](#dependencies)
- [Build and Run Instructions](#build-and-run-instructions)
- [Conclusion](#conclusion)

---

## Overview

The engine is built around a central **Engine** class that manages various **Modules**. Each module (e.g., audio, input, physics) implements a standard interface defined in `Module.h` with methods for initialization, per-frame updates, and shutdown. The engine uses a **ResourceManager** to load and cache shaders, textures (including embedded ones), and models via Assimp and stb_image. A hierarchical scene graph (using **SceneNode** and **Scene**) organizes the scene and supports complex transformations.

---

## Directory Structure and File Descriptions

### Core Engine and Module Management

- **Module.h**  
  Defines the abstract interface for all modules. Each module must implement:
  - `Init()`: Initializes the module.
  - `Update(float dt)`: Called every frame.
  - `Shutdown()`: Cleans up resources.

- **Engine.h / Engine.cpp**  
  The `Engine` class:
  - Maintains a list of modules.
  - Manages the overall initialization, update loop, and shutdown process.
  - Provides methods to add modules and retrieve them by type.

### Logging and File Utilities

- **Logger.h**  
  A thread-safe logging utility that supports multiple log levels (DEBUG, INFO, WARNING, ERROR).  
  - Logs messages to both the console and an optional log file.
  
- **FileUtils.h**  
  Contains helper functions:
  - `NormalizePath()`: Standardizes file paths for cross-platform compatibility.
  - `LoadImageData()`: Loads image data from disk using stb_image, supporting alpha channels.

### Resource Management

- **ResourceManager.h / ResourceManager.cpp**  
  Responsible for loading and caching:
  - **Shaders:** Using the `Shader` class.
  - **Textures:** Using the `Texture2D` class.  
    - **Embedded Textures:** Uses `LoadEmbeddedTexture()` to detect and load textures embedded in models (identified by a path starting with `*`).
  - **Models:** Using the `Model` class.  
  The ResourceManager uses asynchronous loading (with futures) and caches resources to avoid redundant loads.

### Rendering Components

- **Shader.h**  
  Encapsulates shader compilation and linking.  
  - Loads vertex and fragment shader source files.
  - Provides a `Use()` method to activate the shader program.
  
- **Texture2D.h**  
  Wraps the functionality of OpenGL 2D textures.  
  - Generates textures from image data.
  - Manages texture parameters (wrapping, filtering, format).

- **Submesh.h**  
  Represents a subset of a model:
  - Contains vertex data, indices, and material information.
  - Provides methods to set up OpenGL buffers (VAO, VBO, EBO) and to draw the mesh.

- **UniformBuffer.h**  
  Manages OpenGL Uniform Buffer Objects (UBOs):
  - Provides methods to bind, unbind, set data, and bind the UBO to a specific binding point.
  - Used for efficient transmission of data (e.g., lighting parameters) to shaders.

### Model and Mesh Loading

- **Model.h / Model.cpp**  
  The `Model` class:
  - Uses Assimp to load model files (e.g., glTF).
  - Processes meshes to extract vertices, normals, texture coordinates, and tangents.
  - For each mesh, it retrieves material information and uses the helper function `GetTexturePath()` (which now also detects embedded textures) to load textures via the ResourceManager.
  - Builds submeshes and sets up OpenGL buffers.

- **ModelLoader.h / ModelLoader.cpp**  
  Contains utility functions:
  - Converts Assimp matrices (aiMatrix4x4) to glm matrices.
  - Provides a recursive function to process Assimp scene nodes and extract vertex/index data.
  
- **ModelNode.h**  
  A scene node that encapsulates a model:
  - Inherits from `SceneNode`.
  - In its `Render()` method, passes the global transformation matrix to the shader and calls the model’s draw method.

### Scene Graph and Nodes

- **SceneNode.h**  
  Base class for all nodes in the scene graph:
  - Maintains local and global transformation matrices.
  - Supports hierarchical updates (each node updates its children recursively).
  - Provides a virtual `Render()` method.
  
- **Scene.h**  
  Represents the entire scene:
  - Contains a root node (of type `SceneNode`).
  - Calls update and render methods on the scene graph.

### Camera and Player Control

- **Camera.h**  
  Manages the camera's position, orientation, and view matrix:
  - Contains parameters such as position, front, up vectors, yaw, pitch, and mouse sensitivity.
  - Provides methods to compute the view matrix and to process input.

- **PlayerController.h / PlayerController.cpp**  
  Handles player movement and camera follow logic:
  - Processes keyboard input (via GLFW) to update a model’s position and rotation.
  - Adjusts the camera's position and orientation to follow the player.

### Subsystem Modules

- **AudioModule.h**  
  Manages audio system initialization, per-frame updates, and shutdown.
  
- **InputModule.h**  
  Handles input systems (keyboard, mouse, etc.) and logs input update events.
  
- **PhysicsModule.h**  
  Placeholder for physics engine integration; logs initialization and update events.
  
- **RendererModule.h**  
  Responsible for setting up the rendering context (window creation, OpenGL context) and managing the render loop.
  
- **ScriptingModule.h**  
  Integrates a scripting language (e.g., Lua or Python) for runtime logic.
  
- **UIModule.h**  
  Manages user interface elements such as menus and HUD.

### Shaders and External Assets

- **pbr_vertex.glsl**  
  Vertex shader for physically based rendering (PBR).  
  - Computes world positions, normal matrices, and TBN (tangent, bitangent, normal) matrices.
  
- **pbr_fragment.glsl**  
  Fragment shader implementing PBR:
  - Uses albedo, metallic/roughness, and normal maps.
  - Calculates lighting using a microfacet BRDF and supports ambient light.
  - Handles different light types (point and spot lights).
  
- **debug_albedo_fragment.glsl** and **ground_fragment.glsl**  
  Additional shaders used for debugging texture coordinates and rendering ground planes.

- **stb_image.cpp**  
  Implements the stb_image library by defining `STB_IMAGE_IMPLEMENTATION`.

---

## Dependencies

- **GLFW**: Window creation and input handling.
- **GLAD**: OpenGL function loading.
- **glm**: Mathematics for vectors and matrices.
- **Assimp**: Importing various 3D model formats (e.g., glTF).
- **stb_image**: Loading image files.
  
Ensure these libraries are correctly installed and configured in your build system.

---

## Build and Run Instructions

1. **Install Dependencies**:  
   Ensure GLFW, GLAD, glm, Assimp, and stb_image are available and linked in your project.

2. **Build the Project**:  
   Use your preferred build system (CMake, Visual Studio, etc.) to compile all the source files.

3. **Run the Executable**:  
   The application will:
   - Initialize the engine and all modules.
   - Create the window and OpenGL context.
   - Load shaders and models (including automatic detection and loading of embedded textures).
   - Set up the scene graph.
   - Enter the main loop, processing input and rendering the scene.

---