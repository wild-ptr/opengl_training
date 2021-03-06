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
    {-1.5f, -2.2f, -2.5f},
};

typedef struct UniformData
{
    vec3* position;
    size_t box_number;
    Camera* camera;
} UniformData;

static void mvp_transform(Shader* shader, UniformData* data)
{
    Camera* cam = data->camera;

    // simple model transform
    mat4 model_m = GLM_MAT4_IDENTITY_INIT;
    glmc_translate(model_m, *data->position);

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
}

static void calcUniforms_forLightTarget(Shader* shader, void* raw_data)
{
    UniformData* data = raw_data;
    mvp_transform(shader, data);

    shader_setUniformf(shader, "blendFactor", sin(glfwGetTime()));
}

void drawLightScene(Camera* camera)
{
    static Renderable box_scene;

    Shader shader;
    static bool once_flag = false;
    if(!once_flag)
    {
        shader_init_with_f_and_data(&shader,
                                    "src/shaders/SimpleVShader.glsl",
                                    "src/shaders/SimpleFragShader.glsl",
                                    &calcUniforms_forLightTarget,
                                    NULL);

        renderable_create(&box_scene,
                          vertices,
                          sizeof(vertices),
                          NULL,
                          0,
                          NULL,
                          0,
                          &shader);
        once_flag = true;
     }

    UniformData data;
    data.camera = camera;

    for(int i = 0; i < 2; ++i)
    {
        data.box_number = i;
        data.position = &cube_positions[i];
        renderable_up_shader_uni_data(&box_scene, &data);
        renderable_draw(&box_scene);
    }
}
