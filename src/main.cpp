#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vec2.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>
#include <algorithm>

#include"Renderer/Shaderprogram.hpp"
#include"Resources/ResourceManager.hpp"
#include"Renderer/Texture2D.hpp"
#include"Renderer/Sprite.hpp"
#include"Renderer/AnimatedSprite.hpp"
#include"Game/game.hpp"
#include"Renderer/Renderer.hpp"

glm::ivec2 g_windowSize(1152, 768);
game g_game(g_windowSize);


void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
    g_windowSize.x = width; 
    g_windowSize.y = height;
    const float scale = std::min(width / 960.f, height / 640.f);
    const int viewWidth = static_cast<int>(960.f * scale);
    const int viewHeight = static_cast<int>(640.f * scale);
    RendererEngine::Renderer::setViewport(viewWidth, viewHeight, (width - viewWidth) / 2, (height - viewHeight) / 2);
}

void glfwKeyCallback(GLFWwindow* pwindow, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(pwindow, GL_TRUE);
    }
    g_game.setKey(key, action);
}

int main(int argc, char** argv) {

    if (!glfwInit()) {
        std::cout << "GLFW не инициализирован!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* pwindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Tank Arena | WASD / Arrows: move | Space: fire | R: reset | Esc: exit", nullptr, NULL);
    if (!pwindow)
    {
        std::cout << "Ошибка при создании окна!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(pwindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pwindow, glfwKeyCallback);

    glfwMakeContextCurrent(pwindow);

    if (!gladLoadGL()) {
        std::cout << "Не удалось загрузить GLAD" << std::endl;
        return -1;
    }

    glfwSwapInterval(1);
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(pwindow, &framebufferWidth, &framebufferHeight);
    glfwWindowSizeCallback(pwindow, framebufferWidth, framebufferHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Вывод информации о рендерере и версии OpenGL в консоль
    std::cout << "Renderer: " << RendererEngine::Renderer::getRendererStr() << std::endl;
    std::cout << "OpenGL version: " << RendererEngine::Renderer::getVersionStr() << std::endl;

    RendererEngine::Renderer::setClearColor(0, 0, 0, 0);

    {   // изменение области видимости для того что бы обьект ResourceManager успел уничтожиться до выхода из контекста openGL 
        ResourceManager::setExecutablePath(argv[0]);
        if (!g_game.init()) {
            g_game.shutdown();
            ResourceManager::unloadAllResources();
            glfwDestroyWindow(pwindow);
            glfwTerminate();
            return 1;
        }

        auto lastTime = std::chrono::high_resolution_clock::now();

        while (!glfwWindowShouldClose(pwindow))
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            uint64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime).count();
            lastTime = currentTime;
            g_game.update(duration);

            RendererEngine::Renderer::clear();

            g_game.render();


            // Обмен переднего и заднего буферов, чтобы отобразить нарисованное изображение
            glfwSwapBuffers(pwindow);

            glfwPollEvents();
        }
        g_game.shutdown();
        ResourceManager::unloadAllResources();
    }

    glfwTerminate();
    return 0;
}
