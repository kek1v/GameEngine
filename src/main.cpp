#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vec2.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>

#include"Renderer/Shaderprogram.hpp"
#include"Resources/ResourceManager.hpp"
#include"Renderer/Texture2D.hpp"
#include"Renderer/Sprite.hpp"
#include"Renderer/AnimatedSprite.hpp"
#include"Game/game.hpp"

game g_game;
glm::vec2 g_windowSize(640, 480);

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
    g_windowSize.x = width; 
    g_windowSize.y = height;
    glViewport(0, 0, width, height); }

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

    GLFWwindow* pwindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "GameEngine", nullptr, NULL);
    if (!pwindow)
    {
        std::cout << "ќшибка при создании окна!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(pwindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pwindow, glfwKeyCallback);

    glfwMakeContextCurrent(pwindow);

    if (!gladLoadGL()) {
        std::cout << "Ќе удалось загрузить GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ¬ывод информации о рендерере и версии OpenGL в консоль
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(0, 0, 0, 1);

    {   // изменение области видимости дл€ того что бы обьект ResourceManager успел уничтожитьс€ до выхода из контекста openGL 
        ResourceManager::setExecutablePath(argv[0]);
        auto pDefaultShaderProgram = ResourceManager::loadShaders("DefaultShader", "res/shaders/vertex.txt", "res/shaders/fragment.txt");
        if (!pDefaultShaderProgram) {
            std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
            return -1;
        }

        auto pSpriteShaderProgram = ResourceManager::loadShaders("SpriteShader", "res/shaders/vSprite.txt", "res/shaders/fSprite.txt");
        if (!pSpriteShaderProgram) {
            std::cerr << "Can't create sprite shader program: " << "DefaultSprite" << std::endl;
            return -1;
        }

        auto tex = ResourceManager::loadTexture("DefaultTexture", "res/textures/map_16x16.png");

        std::vector<std::string> subTexturesNames = { // см в map16x16        
            "block",
            "topBlock",
            "bottomBlock",
            "leftBlock",
            "rightBlock",
            "topLeftBlock",
            "topRightBlock",
            "bottomLeftBlock",
            "bottomRightBlock",

            "beton",
            "topBeton",
            "bottomBeton",
            "leftBeton",
            "rightBeton",
            "topLeftBeton",
            "topRightBeton",
            "bottomLeftBeton",
            "bottomRightBeton",

            "water1",
			"water2",
		    "water3",

            "trees",
            "ice",
            "wall",            
            "nothing",

            "eagle",
            "deadEagle"

            "respawn1",
            "respawn2",
            "respawn3",
            "respawn4"
        };        
        
        auto pTextureAtlas = ResourceManager::loadTextureAtlas("DefaultTextureAtlas", "res/textures/map_16x16.png", std::move(subTexturesNames), 16, 16);
        
        auto pSprite = ResourceManager::loadSprite("NewSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "ice");
        pSprite->setPosition(glm::vec2(280, 100));

        auto pAnimatedSprite = ResourceManager::loadAnimatedSprite("NewAnimatedSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "water");
        pAnimatedSprite->setPosition(glm::vec2(400, 250));
        std::vector<std::pair<std::string, uint64_t>> waterStates;
        waterStates.emplace_back(std::make_pair<std::string, uint64_t>("water1",1000000000));
        waterStates.emplace_back(std::make_pair<std::string, uint64_t>("water2", 1000000000));
        waterStates.emplace_back(std::make_pair<std::string, uint64_t>("water3", 1000000000));


        std::vector<std::pair<std::string, uint64_t>> eagleStates;
        eagleStates.emplace_back(std::make_pair<std::string, uint64_t>("eagle", 1000000000));
        eagleStates.emplace_back(std::make_pair<std::string, uint64_t>("deadEagle", 1000000000));

        pAnimatedSprite->insertState("waterState", std::move(waterStates));
        pAnimatedSprite->insertState("eagleState", std::move(eagleStates));
        pAnimatedSprite->setState("eagleState");

        pDefaultShaderProgram->use();
        pDefaultShaderProgram->setInt("tex", 0);

        glm::mat4 modelMatrix1 = glm::mat4(1.f); // единична€ матрица (она не выполн€ет никаких преобразований)
        modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(100.f, 200.f, 0.f)); // переместили нашу modelMatrix на vec3

        glm::mat4 modelMatrix2 = glm::mat4(1.f); 
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(540.f, 200.f, 0.f)); 

        // теперь перемножаем на projection matrix
        glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(g_windowSize.x), 0.f, static_cast<float>(g_windowSize.y), -100.f, 100.f);

        pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        pSpriteShaderProgram->use();
        pSpriteShaderProgram->setInt("tex", 0);
        pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        auto lastTime = std::chrono::high_resolution_clock::now();

        while (!glfwWindowShouldClose(pwindow))
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            uint64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime).count();
            lastTime = currentTime;
            pAnimatedSprite->update(duration);

            glClear(GL_COLOR_BUFFER_BIT);

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix1);
            glDrawArrays(GL_TRIANGLES, 0, 3);


            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix2);
            glDrawArrays(GL_TRIANGLES, 0, 3);
    
            pSprite->render();

            pAnimatedSprite->render();

            // ќбмен переднего и заднего буферов, чтобы отобразить нарисованное изображение
            glfwSwapBuffers(pwindow);

            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
