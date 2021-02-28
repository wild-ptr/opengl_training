#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/call.h>
#include <stdio.h>
#include <stdlib.h>
#include "Triangle.h"

GLFWwindow* create_window(void)
{
    GLFWwindow* window = glfwCreateWindow(1500,1200,"learn opengl!",NULL,NULL);
    if(window == NULL)
    {
        printf("Failed to create glfw window.\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    return window;
}

void initialize_glad(void)
{
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("failed to initialize GLAD.\n");
    }
}

void initialize_glfw(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0, width, height);
}

int main(void)
{
    initialize_glfw();
    GLFWwindow* window = create_window();
    initialize_glad();

    glViewport(0, 0, 1500, 1200); // tell opengl the size of viewport. GLFW is not GL.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTriangle();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
