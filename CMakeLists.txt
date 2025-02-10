cmake_minimum_required(VERSION 3.10)

project(Toxic VERSION 1.0 LANGUAGES C CXX)

# Configuración del estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Establece el tipo de compilación predeterminado si no se ha especificado
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Definición del ejecutable y sus fuentes
add_executable(Toxic
    ${CMAKE_SOURCE_DIR}/src/Main.cpp
    ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/ModelLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/ResourceManager.cpp
    ${CMAKE_SOURCE_DIR}/src/stb_image.cpp
    ${CMAKE_SOURCE_DIR}/src/Model.cpp
    ${CMAKE_SOURCE_DIR}/src/Material.cpp
)

# Especifica los directorios de inclusión para Toxic
target_include_directories(Toxic PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs/glad/include
    ${CMAKE_SOURCE_DIR}/libs/assimp/include
    ${CMAKE_SOURCE_DIR}/libs/glfw/include
    ${CMAKE_SOURCE_DIR}/libs/stb/include
    ${CMAKE_SOURCE_DIR}/libs/glm/include
)

# Enlaza las bibliotecas necesarias para Toxic
target_link_libraries(Toxic PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/assimp/lib/assimp-vc143-mt.lib
    opengl32
    gdi32
    ${CMAKE_SOURCE_DIR}/libs/glfw/lib/glfw3.lib
    ${CMAKE_SOURCE_DIR}/libs/glm/lib/glm.lib
)

# Copia la DLL de Assimp al directorio del ejecutable después de la compilación (solo en Windows)
if(WIN32)
    add_custom_command(TARGET Toxic POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/libs/assimp/bin/assimp-vc143-mt.dll
            $<TARGET_FILE_DIR:Toxic>
        COMMENT "Copiando la DLL de Assimp al directorio del ejecutable"
    )
endif()




