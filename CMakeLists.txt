cmake_minimum_required(VERSION 3.10)

project(Toxic VERSION 1.0 LANGUAGES C CXX)

# Set the C++ standard to C++17 and make it required.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the default build type to Release if it has not been specified.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Define the executable and its source files.
add_executable(Toxic
    ${CMAKE_SOURCE_DIR}/src/Main.cpp
    ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/ResourceManager.cpp
    ${CMAKE_SOURCE_DIR}/src/stb_image.cpp
    ${CMAKE_SOURCE_DIR}/src/Model.cpp
    ${CMAKE_SOURCE_DIR}/src/EntityLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/ModelLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/Config.cpp
)

# Specify the include directories for Toxic.
target_include_directories(Toxic PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs/glad/include
    ${CMAKE_SOURCE_DIR}/libs/assimp/include
    ${CMAKE_SOURCE_DIR}/libs/glfw/include
    ${CMAKE_SOURCE_DIR}/libs/stb/include
    ${CMAKE_SOURCE_DIR}/libs/glm/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/yaml/include
    ${CMAKE_SOURCE_DIR}/libs/tinygltf/include
)

# Link the necessary libraries for Toxic.
target_link_libraries(Toxic PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/assimp/lib/assimp-vc143-mt.lib
    opengl32
    gdi32
    ${CMAKE_SOURCE_DIR}/libs/glfw/lib/glfw3.lib
    ${CMAKE_SOURCE_DIR}/libs/glm/lib/glm.lib
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/libs/yamldebug/lib/yaml-cppd.lib>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/libs/yaml/lib/yaml-cpp.lib>
)

target_compile_definitions(Toxic PRIVATE YAML_CPP_STATIC_DEFINE)

# On Windows, copy the Assimp DLL to the executable directory after compilation.
if(WIN32)
    add_custom_command(TARGET Toxic POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/libs/assimp/bin/assimp-vc143-mt.dll
            $<TARGET_FILE_DIR:Toxic>
        COMMENT "Copying Assimp DLL to the executable directory"
    )
endif()





