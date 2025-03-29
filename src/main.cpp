#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include"Renderer/Shaderprogram.hpp"

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

// �������� ��� ���������� ������� �� GLSL
// ����� ������������ ������� ������� � ���� �������,
// � ��������� ���������� � ������������.
const char* vertex_shader =
"#version 460\n"                         // ��������� ������ GLSL
"layout(location = 0) in vec3 vertex_position;"  // ������� �������: ������� ������� (location = 0)
"layout(location = 1) in vec3 vertex_color;"     // ������� �������: ���� ������� (location = 1)
"out vec3 color;"                         // �������� �������, ���������� ���� � ����������� ������
"void main(){"
"   color = vertex_color;"               // �������� ���� ������� ������
"   gl_Position = vec4(vertex_position, 1.0);" // ������������� ������� ������� (w = 1.0 ��� ���������� ���������)
"}";

// �������� ��� ������������ ������� �� GLSL
// �������� ���� �� ���������� ������� � ������������� ��� ��� �������� ���� �������
const char* fragment_shader =
"#version 460\n"                         // ��������� ������ GLSL
"in vec3 color;"                          // ������� ������� - ����, ���������� �� ���������� �������
"out vec4 frag_color;"                    // �������� ���� ���������
"void main(){"
"   frag_color = vec4(color, 1.0);"        // ������������� ���� �������, alpha = 1.0 (������������)
"}";

int main(void)
{
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

    std::string vertexShader(vertex_shader);
    std::string fragmentShader(fragment_shader);
    Renderer::ShaderProgram shaderprogram(vertexShader, fragmentShader);
    if (!shaderprogram.isCompiled()) {
        std::cerr << "Can't create shader program" << std::endl;
        return -1;
    }


    // ������� ����� ��� �������� ��������� ��������� (VBO ��� �����)
    GLuint points_vbo = 0;
    glGenBuffers(1, &points_vbo); // ���������� ������������� ������
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // ����������� ����� � ���� GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); // ��������� ������ ������

    // ������� ����� ��� �������� ������ ������ (VBO ��� ������)
    GLuint colors_vbo = 0;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW); // ��������� ������ ������

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
    glEnableVertexAttribArray(1); // �������� ������� � location = 1 (����)
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo); // ����������� VBO ��� ������
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // ���������� ������ ������ ��� ������

    // �������� ���� ����������: �����������, ���� ���� �� ����� �������
    while (!glfwWindowShouldClose(pwindow))
    {
        // ������� ������: ������� ����� �����, ��������� �������� ����� glClearColor
        glClear(GL_COLOR_BUFFER_BIT);
        
        shaderprogram.use();
        // ����������� VAO, ���������� ��������� ��� ������ � ������
        glBindVertexArray(vao);
        // ������ �����������: GL_TRIANGLES ���������, ��� ������ 3 ������� ���������� ���� �����������
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // ����� ��������� � ������� �������, ����� ���������� ������������ �����������
        glfwSwapBuffers(pwindow);

        // ��������� ������� (��������, ���� � ����������, ��������� �������� ����)
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
