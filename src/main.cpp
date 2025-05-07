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

glm::vec2 g_windowSize(640, 480);
bool isGrass = false;

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
    g_windowSize.x = width; 
    g_windowSize.y = height;
    glViewport(0, 0, width, height); }

void glfwKeyCallback(GLFWwindow* pwindow, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(pwindow, GL_TRUE);
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        isGrass = !isGrass;
    }
}

GLfloat point[] = {
     0.0f,  50.f, 0.0f,
     50.f, -50.f, 0.0f,
    -50.f, -50.f, 0.0f
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,   
    0.0f, 1.0f, 0.0f,   
    0.0f, 0.0f, 1.0f    
};

GLfloat texCoords[] = {
    0.5f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
};


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
        std::cout << "Ошибка при создании окна!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(pwindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pwindow, glfwKeyCallback);

    glfwMakeContextCurrent(pwindow);

    if (!gladLoadGL()) {
        std::cout << "Не удалось загрузить GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Вывод информации о рендерере и версии OpenGL в консоль
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(100, 0, 52, 1);

    {   // изменение области видимости для того что бы обьект ResourceManager успел уничтожиться до выхода из контекста openGL 
        ResourceManager resourceManager(argv[0]);
        auto pDefaultShaderProgram = resourceManager.loadShaders("DefaultShader", "res/shaders/vertex.txt", "res/shaders/fragment.txt");
        if (!pDefaultShaderProgram) {
            std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
            return -1;
        }

        auto pSpriteShaderProgram = resourceManager.loadShaders("SpriteShader", "res/shaders/vSprite.txt", "res/shaders/fSprite.txt");
        if (!pSpriteShaderProgram) {
            std::cerr << "Can't create sprite shader program: " << "DefaultSprite" << std::endl;
            return -1;
        }

        auto tex = resourceManager.loadTexture("DefaultTexture", "res/textures/triangle_texture.jpg");

        std::vector<std::string> subTexturesNames = { // см в tex_atlas1.png
            // Ряд 1
            "sand",             // Песок
            "wood_bark",        // Древесная кора
            "cliff_dark",       // Темная скала
            "cliff_striated",   // Слоистая скала
            // Ряд 2
            "gravel",           // Гравий/Асфальт
            "grass_sparse",     // Редкая трава
            "grass_lush",       // Густая трава
            "mossy_ground",     // Мшистая земля
            // Ряд 3
            "dry_leaves",       // Сухие листья/Лесная подстилка
            "soil_gravel",      // Почва с гравием
            "rock_red",         // Красный камень/Скала
            "rocky_ground",     // Каменистая земля
            // Ряд 4
            "snow",             // Снег
            "stone_dark",       // Темный камень
            "stone_wall",       // Каменная стена
            "cobblestone"       // Брусчатка/Булыжник
        };        
        
        auto pTextureAtlas = resourceManager.loadTextureAtlas("DefaultTextureAtlas", "res/textures/tex_atlas1.png", std::move(subTexturesNames), 128, 128);
        
        auto pSprite = resourceManager.loadSprite("NewSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "wood_bark");
        pSprite->setPosition(glm::vec2(280, 100));

        auto pAnimatedSprite = resourceManager.loadAnimatedSprite("NewAnimatedSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "rock_red");
        pAnimatedSprite->setPosition(glm::vec2(400, 250));
        std::vector<std::pair<std::string, uint64_t>> rockStates;
        rockStates.emplace_back(std::make_pair<std::string, uint64_t>("rock_red",1000000000));
        rockStates.emplace_back(std::make_pair<std::string, uint64_t>("rocky_ground", 1000000000));

        std::vector<std::pair<std::string, uint64_t>> stoneStates;
        stoneStates.emplace_back(std::make_pair<std::string, uint64_t>("stone_dark", 1000000000));
        stoneStates.emplace_back(std::make_pair<std::string, uint64_t>("stone_wall", 1000000000));

        pAnimatedSprite->insertState("rockState", std::move(rockStates));
        pAnimatedSprite->insertState("stoneState", std::move(stoneStates));
        pAnimatedSprite->setState("stoneState");

        // Создаем буфер для хранения вершинных координат (VBO для точек)
        GLuint points_vbo = 0;
        glGenBuffers(1, &points_vbo); // Генерируем идентификатор буфера
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // Привязываем буфер к типу GL_ARRAY_BUFFER
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); // Загружаем данные вершин

        glGenBuffers(1, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

        // Создаем буфер для хранения цветов вершин (VBO для цветов)
        GLuint texCoord_vbo = 0;
        glGenBuffers(1, &texCoord_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW); // Загружаем данные цветов

        // Создаем объект вершинного массива (VAO)
        // VAO хранит информацию о том, как привязаны VBO и их атрибуты
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Настраиваем атрибут для вершин (позиция):
        glEnableVertexAttribArray(0); // Включаем атрибут с location = 0 (позиция)
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // Привязываем соответствующий VBO
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Определяем формат данных:
        // 3 компоненты на вершину, тип данных GL_FLOAT, без нормализации, без промежутка между элементами

        // Настраиваем атрибут для цветов:
        GLuint colors_vbo = 0;
        glEnableVertexAttribArray(1); // Включаем атрибут с location = 1 (цвет)
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo); // Привязываем VBO для цветов
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Определяем формат данных для цветов

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        pDefaultShaderProgram->use();
        pDefaultShaderProgram->setInt("tex", 0);



        glm::mat4 modelMatrix1 = glm::mat4(1.f); // единичная матрица (она не выполняет никаких преобразований)
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

            if (isGrass) {
                pAnimatedSprite->setState("rockState");
            }
            else {
                pAnimatedSprite->setState("stoneState");
            }

            auto currentTime = std::chrono::high_resolution_clock::now();
            uint64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime).count();
            lastTime = currentTime;
            pAnimatedSprite->update(duration);

            glClear(GL_COLOR_BUFFER_BIT);

            pDefaultShaderProgram->use();
            glBindVertexArray(vao);
            tex->bind();            

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix1);
            glDrawArrays(GL_TRIANGLES, 0, 3);


            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix2);
            glDrawArrays(GL_TRIANGLES, 0, 3);
    
            pSprite->render();

            pAnimatedSprite->render();

            // Обмен переднего и заднего буферов, чтобы отобразить нарисованное изображение
            glfwSwapBuffers(pwindow);

            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
