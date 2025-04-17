#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vec2.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>

#include"Renderer/Shaderprogram.hpp"
#include"Resources/ResourceManager.hpp"
#include"Renderer/Texture2D.hpp"
#include"Renderer/Sprite.hpp"

glm::ivec2 g_windowSize(640, 480);

// ������� ��������� ������ ��� ��������� ��������� �������� ����
// ������ ���, ����� ���� �������� ������, ��������� ���������� ���������� � ������� ���������
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
    g_windowSize.x = width; 
    g_windowSize.y = height;
    // ������������� ������� ������ �� ����� ����
    glViewport(0, 0, width, height); // ���������� (0,0) - ������ ����� ����, ����� ������ � ������
}

// ������� ��������� ������ ��� ��������� ������� ������
// ���� ������ ������� Escape, ������������� ���� �������� ����
void glfwKeyCallback(GLFWwindow* pwindow, int key, int scancode, int action, int mode) {
    // ���� ������ ������� ESC � �������� - ������� (PRESS)
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // ������������� ����, ����� �������� ���� ����, ��� ���� ��������� ����
        glfwSetWindowShouldClose(pwindow, GL_TRUE);
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
        std::cout << "GLFW �� ���������������!" << std::endl;
        return -1;
    }

    // ������������� ��������� ��� ������������ ����:
    // ��������� ��������� ������ OpenGL (4.6) � ������� (core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* pwindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "GameEngine", nullptr, NULL);
    if (!pwindow)
    {
        std::cout << "������ ��� �������� ����!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(pwindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pwindow, glfwKeyCallback);

    glfwMakeContextCurrent(pwindow);

    if (!gladLoadGL()) {
        std::cout << "�� ������� ��������� GLAD" << std::endl;
        return -1;
    }

    // ����� ���������� � ��������� � ������ OpenGL � �������
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // ������������� ���� ������� ������ (������� ���)
    glClearColor(100, 0, 52, 1);

    {   // ��������� ������� ��������� ��� ���� ��� �� ������ ResourceManager ����� ������������ �� ������ �� ��������� openGL 
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

        std::vector<std::string> subTexturesNames = { // �� � tex_atlas1.png
            // ��� 1
            "sand",             // �����
            "wood_bark",        // ��������� ����
            "cliff_dark",       // ������ �����
            "cliff_striated",   // �������� �����
            // ��� 2
            "gravel",           // ������/�������
            "grass_sparse",     // ������ �����
            "grass_lush",       // ������ �����
            "mossy_ground",     // ������� �����
            // ��� 3
            "dry_leaves",       // ����� ������/������ ���������
            "soil_gravel",      // ����� � �������
            "rock_red",         // ������� ������/�����
            "rocky_ground",     // ���������� �����
            // ��� 4
            "snow",             // ����
            "stone_dark",       // ������ ������
            "stone_wall",       // �������� �����
            "cobblestone"       // ���������/��������
        };        
        
        auto pTextureAtlas = resourceManager.loadTextureAtlas("DefaultTextureAtlas", "res/textures/tex_atlas1.png", std::move(subTexturesNames), 256, 256);
        
        auto pSprite = resourceManager.loadSprite("NewSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "cobblestone");
        pSprite->setPosition(glm::vec2(300, 100));

        // ������� ����� ��� �������� ��������� ��������� (VBO ��� �����)
        GLuint points_vbo = 0;
        glGenBuffers(1, &points_vbo); // ���������� ������������� ������
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // ����������� ����� � ���� GL_ARRAY_BUFFER
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); // ��������� ������ ������

        glGenBuffers(1, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

        // ������� ����� ��� �������� ������ ������ (VBO ��� ������)
        GLuint texCoord_vbo = 0;
        glGenBuffers(1, &texCoord_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW); // ��������� ������ ������

        // ������� ������ ���������� ������� (VAO)
        // VAO ������ ���������� � ���, ��� ��������� VBO � �� ��������
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // ����������� ������� ��� ������ (�������):
        glEnableVertexAttribArray(0); // �������� ������� � location = 0 (�������)
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // ����������� ��������������� VBO
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // ���������� ������ ������:
        // 3 ���������� �� �������, ��� ������ GL_FLOAT, ��� ������������, ��� ���������� ����� ����������

        // ����������� ������� ��� ������:
        GLuint colors_vbo = 0;
        glEnableVertexAttribArray(1); // �������� ������� � location = 1 (����)
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo); // ����������� VBO ��� ������
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // ���������� ������ ������ ��� ������

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        pDefaultShaderProgram->use();
        pDefaultShaderProgram->setInt("tex", 0);



        glm::mat4 modelMatrix1 = glm::mat4(1.f); // ��������� ������� (��� �� ��������� ������� ��������������)
        modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(100.f, 200.f, 0.f)); // ����������� ���� modelMatrix �� vec3

        glm::mat4 modelMatrix2 = glm::mat4(1.f); 
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(540.f, 200.f, 0.f)); 

        // ������ ����������� �� projection matrix
        glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(g_windowSize.x), 0.f, static_cast<float>(g_windowSize.y), -100.f, 100.f);

        pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        pSpriteShaderProgram->use();
        pSpriteShaderProgram->setInt("tex", 0);
        pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        // �������� ���� ����������: �����������, ���� ���� �� ����� �������
        while (!glfwWindowShouldClose(pwindow))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            pDefaultShaderProgram->use();
            // ����������� VAO, ���������� ��������� ��� ������ � ������
            glBindVertexArray(vao);
            tex->bind(); // ������ �������� ��������

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix1);
            glDrawArrays(GL_TRIANGLES, 0, 3);


            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix2);
            glDrawArrays(GL_TRIANGLES, 0, 3);
    
            pSprite->render();

            // ����� ��������� � ������� �������, ����� ���������� ������������ �����������
            glfwSwapBuffers(pwindow);

            // ��������� ������� (��������, ���� � ����������, ��������� �������� ����)
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
