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
    Camera* camera;
	vec3* scaleFactor;
} UniformData;

static void mvp_transform(Shader* shader, UniformData* data)
{
    Camera* cam = data->camera;

    // simple model transform
    mat4 model_m = GLM_MAT4_IDENTITY_INIT;
    glmc_translate(model_m, *data->position);
	glmc_scale(model_m, *data->scaleFactor);

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

	static vec3 obj_col = {1.0f, 0.5f, 0.31f};
	static vec3 light_col = {1.0f, 1.0f, 1.0f};
	shader_setUniform3vec(shader, "objectColor", (vec3){0.3f, 0.0f, 0.6f});
	shader_setUniform3vec(shader, "lightColor", light_col);
}

static void calcUniforms_forLightSource(Shader* shader, void* raw_data)
{
    UniformData* data = raw_data;
    mvp_transform(shader, data);
}

Renderable create_light_target()
{
	Shader shader;
    shader_init_with_f_and_data(&shader,
                                "src/shaders/LightningVShader.glsl",
                                "src/shaders/LightningFragShader.glsl",
                                &calcUniforms_forLightTarget,
                                NULL);

	Renderable light_target;
    renderable_create(&light_target,
                      vertices,
                      sizeof(vertices),
                      NULL,
                      0,
                      NULL,
                      0,
                      &shader);


	return light_target;
}

Renderable create_light_source()
{
	Shader shader;
    shader_init_with_f_and_data(&shader,
                                "src/shaders/LightningVShader.glsl",
                                "src/shaders/LightSourceFShader.glsl",
                                &calcUniforms_forLightSource,
                                NULL);

	Renderable light_source;
    renderable_create(&light_source,
                      vertices,
                      sizeof(vertices),
                      NULL,
                      0,
                      NULL,
                      0,
                      &shader);
	return light_source;
}

void drawLightScene(Camera* camera)
{
    static Renderable src;
    static Renderable target;

    static bool once_flag = false;
    if(!once_flag)
    {
		src = create_light_source();
		target = create_light_target();
		once_flag = true;
    }

    UniformData data_src;
	UniformData data_target;

    data_src.camera = camera;
	data_target.camera = camera;

	data_src.position = &cube_positions[0];
	data_target.position = &cube_positions[1];

	vec3 scale_f_src = {0.5, 0.5, 0.5};
	vec3 scale_f_target = {1.5, 1.5, 1.5};
	data_src.scaleFactor = &scale_f_src;
	data_target.scaleFactor = &scale_f_target;

    renderable_up_shader_uni_data(&src, &data_src);
    renderable_up_shader_uni_data(&target, &data_target);

    renderable_draw(&src);
    renderable_draw(&target);
}
