#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int g_windowSizeX = 640;
int g_windowSizeY = 480;

void glfwWindowSizeCallback(GLFWwindow* x, int width, int height) {
    g_windowSizeX = width;
    g_windowSizeY = height;
    glViewport(0, 0, g_windowSizeX, g_windowSizeY); // выбираем где будем рисовать (с левого ниженого края с высотой х и шириной у)
}

void glfwKeyCallback(GLFWwindow* pwindow, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(pwindow, GL_TRUE); // renderer loop вернет false и произойдет выход из него
    }
}

int main(void)
{

    /* Initialize the library */
    if (!glfwInit()) {
        std::cout << "GLFW don't init! " << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a pwindowed mode pwindow and its OpenGL context */
    GLFWwindow* pwindow = glfwCreateWindow(g_windowSizeX,g_windowSizeY, "GameEngine", nullptr, NULL);
    if (!pwindow)
    {
        std::cout << "glfwCreateWindow failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(pwindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pwindow, glfwKeyCallback);

    /* Make the pwindow's context current */
    glfwMakeContextCurrent(pwindow);

	if(!gladLoadGL()){
		std::cout << "Can't load GLAD" << std::endl;
		return -1;
	}

    std::cout << "Renderer" << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version" << glGetString(GL_VERSION) << std::endl;

	glClearColor(0 , 1, 0, 1);

    /* Loop until the user closes the pwindow */
    while (!glfwWindowShouldClose(pwindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(pwindow);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
