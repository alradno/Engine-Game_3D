cmake_minimum_required(VERSION 3.10)
project(ToxicTest VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SceneSwitchingTest
    ${CMAKE_SOURCE_DIR}/test/SceneSwitchingTest.cpp
    ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/stb_image.cpp
    ${CMAKE_SOURCE_DIR}/src/Config.cpp
    ${CMAKE_SOURCE_DIR}/src/ResourceManager.cpp
    ${CMAKE_SOURCE_DIR}/src/EntityLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/RenderSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/SceneResources.cpp
    ${CMAKE_SOURCE_DIR}/src/Model.cpp
    ${CMAKE_SOURCE_DIR}/src/ModelLoader.cpp
    ${CMAKE_SOURCE_DIR}/scenes/Scene1.cpp
    ${CMAKE_SOURCE_DIR}/scenes/Scene2.cpp
)

target_include_directories(SceneSwitchingTest PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/glad/include
    ${CMAKE_SOURCE_DIR}/libs/assimp/include
    ${CMAKE_SOURCE_DIR}/libs/glfw/include
    ${CMAKE_SOURCE_DIR}/libs/stb/include
    ${CMAKE_SOURCE_DIR}/libs/glm/include
    ${CMAKE_SOURCE_DIR}/libs/yaml/include
    ${CMAKE_SOURCE_DIR}/libs/freetype/include
)

target_link_libraries(SceneSwitchingTest PRIVATE
    opengl32
    gdi32
    ${CMAKE_SOURCE_DIR}/libs/assimp/lib/assimp-vc143-mt.lib
    ${CMAKE_SOURCE_DIR}/libs/freetype/lib/freetype.lib
    ${CMAKE_SOURCE_DIR}/libs/glfw/lib/glfw3.lib
    ${CMAKE_SOURCE_DIR}/libs/glm/lib/glm.lib
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/libs/yamldebug/lib/yaml-cppd.lib>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/libs/yaml/lib/yaml-cpp.lib>
)

target_compile_definitions(SceneSwitchingTest PRIVATE YAML_CPP_STATIC_DEFINE)

if(WIN32)
    add_custom_command(TARGET SceneSwitchingTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/libs/assimp/bin/assimp-vc143-mt.dll
            $<TARGET_FILE_DIR:SceneSwitchingTest>
        COMMENT "Copying Assimp DLL to the executable directory"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/libs/freetype/bin/freetype.dll
            $<TARGET_FILE_DIR:SceneSwitchingTest>
        COMMENT "Copying FreeType DLL to the executable directory"
    )
endif()