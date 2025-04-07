#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include"Renderer/Shaderprogram.hpp"
#include"Resources/ResourceManager.hpp"
#include"Renderer/Texture2D.hpp"

int g_windowSizeX = 640;
int g_windowSizeY = 480;

// ������� ��������� ������ ��� ��������� ��������� �������� ����
// ������ ���, ����� ���� �������� ������, ��������� ���������� ���������� � ������� ���������
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
    g_windowSizeX = width; 
    g_windowSizeY = height;
    // ������������� ������� ������ �� ����� ����
    glViewport(0, 0, g_windowSizeX, g_windowSizeY); // ���������� (0,0) - ������ ����� ����, ����� ������ � ������
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
     0.0f,  0.5f, 0.0f,  
     0.5f, -0.5f, 0.0f,  
    -0.5f, -0.5f, 0.0f   
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

    GLFWwindow* pwindow = glfwCreateWindow(g_windowSizeX, g_windowSizeY, "GameEngine", nullptr, NULL);
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
    glClearColor(0, 1, 0, 1);

    {   // ��������� ������� ��������� ��� ���� ��� �� ������ ResourceManager ����� ������������ �� ������ �� ��������� openGL 
        ResourceManager resourceManager(argv[0]);
        auto pDefaultShaderProgram = resourceManager.loadShaders("DefaultShader", "res/shaders/vertex.txt", "res/shaders/fragment.txt");
        if (!pDefaultShaderProgram) {
            std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
            return 1;
        }

        auto tex = resourceManager.loadTexture("DefaultTexture", "res/textures/triangle_texture.jpg");

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

        // �������� ���� ����������: �����������, ���� ���� �� ����� �������
        while (!glfwWindowShouldClose(pwindow))
        {
            // ������� ������: ������� ����� �����, ��������� �������� ����� glClearColor
            glClear(GL_COLOR_BUFFER_BIT);

            pDefaultShaderProgram->use();
            // ����������� VAO, ���������� ��������� ��� ������ � ������
            glBindVertexArray(vao);
            tex->bind(); // ������ �������� ��������
            // ������ �����������: GL_TRIANGLES ���������, ��� ������ 3 ������� ���������� ���� �����������
            glDrawArrays(GL_TRIANGLES, 0, 3);

            // ����� ��������� � ������� �������, ����� ���������� ������������ �����������
            glfwSwapBuffers(pwindow);

            // ��������� ������� (��������, ���� � ����������, ��������� �������� ����)
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
