cmake_minimum_required(VERSION 3.29 FATAL_ERROR)

set(PROJECT_NAME GameEngine)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDART 17)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/Game/game.cpp
    src/Game/game.hpp
    src/Game/Tank.cpp
    src/Game/Tank.hpp
    src/Renderer/VertexBuffer.cpp
    src/Renderer/VertexBuffer.hpp
    src/Renderer/IndexBuffer.cpp
    src/Renderer/IndexBuffer.hpp
    src/Renderer/ShaderProgram.cpp
    src/Renderer/ShaderProgram.hpp
    src/Renderer/Texture2D.cpp
    src/Renderer/Texture2D.hpp
    src/Renderer/Sprite.hpp
    src/Renderer/Sprite.cpp
    src/Renderer/AnimatedSprite.cpp
    src/Renderer/AnimatedSprite.hpp
    src/Resources/ResourceManager.cpp
    src/Resources/ResourceManager.hpp
    src/Resources/stb_image.h)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

include_directories(external/glm)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)