#include "BoxScene.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderable.h"
#include "Camera.h"
#include <cglm/call.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static float vertices[] =
{
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

static vec3 cube_positions[] = {
    { 0.0f,  0.0f,  0.0f},
    { 2.0f,  5.0f, -15.0f},
    {-1.5f, -2.2f, -2.5f},
    {-3.8f, -2.0f, -12.3f},
    { 2.4f, -0.4f, -3.5f},
    {-1.7f,  3.0f, -7.5f},
    { 1.3f, -2.0f, -2.5f},
    { 1.5f,  2.0f, -2.5f},
    { 1.5f,  0.2f, -1.5f},
    {-1.3f,  1.0f, -1.5f}
};

typedef struct UniformData
{
    vec3* position;
    size_t box_number;
    Camera* camera;
} UniformData;

static void calcUniforms(Shader* shader, void* raw_data)
{
    UniformData* data = raw_data;
    Camera* cam = data->camera;
    int i = data->box_number;

    // simple model transform
    mat4 model_m = GLM_MAT4_IDENTITY_INIT;
    glmc_translate(model_m, *data->position);

    if(i%3 == 0)
        glmc_rotate(model_m, glm_rad(-45.0f)*(float)glfwGetTime(), (vec3){1.0f, 1.0f, 0.0f});
    else
        glmc_rotate(model_m, glm_rad(15.0f * i), (vec3){1.0f, 0.0f, 0.0f});

    // view transform on world space to view space
    mat4 view_m = GLM_MAT4_IDENTITY_INIT;
    vec3 dir_v = {0, 0, 0};
    glm_vec3_add(cam->pos_v, cam->dir_v, dir_v);
    glm_lookat(cam->pos_v,
               dir_v, // added pos and dir vec
               cam->up_v,
               view_m);

    // perspective projection transform from view space to clip space
    mat4 proj_m = GLM_MAT4_IDENTITY_INIT;
    glmc_perspective(glm_rad(45.0f), 1500.0f/1200.0f, 0.1f, 100.0f, proj_m);

    //sets
    shader_setUniform4mat(shader, "model_m", &model_m);
    shader_setUniform4mat(shader, "view_m", &view_m);
    shader_setUniform4mat(shader, "proj_m", &proj_m);

    shader_setUniformf(shader, "blendFactor", sin(glfwGetTime()));

    shader_setUniformi(shader, "usedTexture", 0);
    shader_setUniformi(shader, "usedTexture2", 1);
}

static void setVertexAttributes()
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default.
    glEnableVertexAttribArray(1);
}

void drawBoxScene(Camera* camera)
{
    static Renderable box_scene;

    Texture textures[2];
    Shader shader;
    static bool once_flag = false;
    if(!once_flag)
    {
        texture_init_rgb(&textures[0], "assets/container.jpg", GL_RGB);
        texture_init_rgb(&textures[1], "assets/awesomeface.png", GL_RGBA);

        shader_init_with_f_and_data(&shader,
                                    "src/shaders/SimpleVShader.glsl",
                                    "src/shaders/SimpleFragShader.glsl",
                                    &calcUniforms,
                                    NULL);

        renderable_create(&box_scene,
                          setVertexAttributes,
                          vertices,
                          sizeof(vertices),
                          NULL,
                          0,
                          textures,
                          2,
                          &shader);
        once_flag = true;
     }

    UniformData data;
    data.camera = camera;

    for(int i = 0; i < 10; ++i)
    {
        data.box_number = i;
        data.position = &cube_positions[i];
        renderable_up_shader_uni_data(&box_scene, &data);
        renderable_draw(&box_scene);
    }
}
