#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/call.h>
#include <stdio.h>
#include <stdlib.h>
//#include "Triangle.h"
#include "BoxScene.h"
#include "Camera.h"

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

void process_input(GLFWwindow* window, Camera* cam)
{
    // all of this belongs in the camera class,
    // @TODO: process_input should have no fucking idea
    // about how all this vector shit works, it should just
    // moveForward(amount) instead of this.
    // to move later.
    float speed = cam->cam_speed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        vec3 scaled;
        glm_vec3_scale(cam->front_v, speed, scaled);
        glm_vec3_add(cam->pos_v, scaled, cam->pos_v);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        vec3 scaled;
        glm_vec3_scale(cam->front_v, speed, scaled);
        glm_vec3_sub(cam->pos_v, scaled, cam->pos_v);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        vec3 scaled;
        glm_vec3_cross(cam->front_v, cam->up_v, scaled);
        glm_normalize(scaled);
        glm_vec3_scale(scaled, speed, scaled);
        glm_vec3_sub(cam->pos_v, scaled, cam->pos_v);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 scaled;
        glm_vec3_cross(cam->front_v, cam->up_v, scaled);
        glm_normalize(scaled);
        glm_vec3_scale(scaled, speed, scaled);
        glm_vec3_add(cam->pos_v, scaled, cam->pos_v);
    }
}

int main(void)
{
    initialize_glfw();
    GLFWwindow* window = create_window();
    initialize_glad();

    glViewport(0, 0, 1500, 1200); // tell opengl the size of viewport. GLFW is not GL.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    Camera camera;
    camera_init(&camera);
    camera.cam_speed = 0.2f;
    camera.pos_v[2] = 3.0f; // (0,0,3) position
    camera.front_v[2] = -1.0f; // (0,0,-1) front
    camera.up_v[1] = 1.0f; // (0,1,0) up v of world space;

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        process_input(window, &camera);

        drawBoxScene(&camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
