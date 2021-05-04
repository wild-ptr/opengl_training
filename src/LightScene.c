#include "BoxScene.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderable.h"
#include "Camera.h"
#include <cglm/call.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include "Utilities.h"


static float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

static vec3 cube_positions[] = {
    { 0.0f,  0.0f,  0.0f},
    {-1.5f, -2.2f, -2.5f},
};


static Light light = {
    .ambient = {0.15, 0.15, 0.15},
    .diffuse = {0.6, 0.6, 0.6},
    .specular = {0.5, 0.5, 0.5}
    // . position uninitialized
};

typedef struct UniformData
{
    vec3 position;
    Camera* camera;
	vec3* scaleFactor;
} UniformData;

static void mvp_transform(Shader* shader, const UniformData* data)
{
    Camera* cam = data->camera;

    // simple model transform
    mat4 model_m = GLM_MAT4_IDENTITY_INIT;
    glmc_translate(model_m, data->position);
	glmc_scale(model_m, *data->scaleFactor);
    glmc_rotate(model_m, glfwGetTime(), (vec3){1.0f, 0.0f, 0.0f});

    // normal transform matrix
    mat3 norm_m;
    glmc_mat4_pick3(model_m, norm_m);
    glmc_mat3_transpose(norm_m);
    glmc_mat3_inv(norm_m, norm_m);

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
    shader_setUniform3mat(shader, "norm_m", &norm_m);
}

static void calcUniforms_forLightTarget(Shader* shader, void* raw_data)
{
    const UniformData* data = raw_data;
    mvp_transform(shader, data);

    shader_setUniform3vec(shader, "viewPos", data->camera->pos_v);
    shader_set_light(shader, &light, "light");
    shader_setUniformf(shader, "time", glfwGetTime());
}


static void calcUniforms_forLightSource(Shader* shader, void* raw_data)
{
    UniformData* data = raw_data;
    static Material cube_material;

    mvp_transform(shader, data);
}

static void set_vertex_attributes()
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

static Renderable create_light_target()
{
	Shader shader;
    shader_init_with_f_and_data(&shader,
                                "src/shaders/LightningVShader.glsl",
                                "src/shaders/LightningFragShader.glsl",
                                &calcUniforms_forLightTarget,
                                NULL);

    Material cube_materials[2] = {
        { .shininess = 32.0f },
        { .shininess = 32.0f }
    };

    //material_init(&cube_material,
    //              "assets/container2.png", GL_RGBA,
    //              "assets/container2_specular.png", GL_RGBA,
    //              "assets/normal_bricks.jpg", GL_RGB);
    material_init(&cube_materials[0],
                  "assets/bricks.jpg", GL_RGB,
                  "assets/container2_specular.png", GL_RGBA,
                  "assets/normal_bricks.jpg", GL_RGB);

    material_init(&cube_materials[1],
                  "assets/matrix.jpg", GL_RGB,
                  NULL, GL_RGBA,
                  NULL, GL_RGB);

	Renderable light_target;
    renderable_create(&light_target,
                      set_vertex_attributes,
                      vertices,
                      sizeof(vertices),
                      NULL,
                      0,
                      cube_materials,
                      2,
                      &shader);

	return light_target;
}

static Renderable create_light_source()
{
	Shader shader;
    shader_init_with_f_and_data(&shader,
                                "src/shaders/LightningVShader.glsl",
                                "src/shaders/LightSourceFShader.glsl",
                                &calcUniforms_forLightSource,
                                NULL);

	Renderable light_source;
    renderable_create(&light_source,
                      set_vertex_attributes,
                      vertices,
                      sizeof(vertices),
                      NULL,
                      0,
                      NULL,
                      0,
                      &shader);

    set_vertex_attributes();
	return light_source;
}

void drawLightScene(Camera* camera)
{
    static Renderable src;
    static Renderable target;

    CALL_ONCE
    {
		src = create_light_source();
		target = create_light_target();
        CALL_ONCE_END;
    }

    UniformData data_src;
	UniformData data_target;

    data_src.camera = camera;
	data_target.camera = camera;

    const float radius = 3.0f;
    const float camX = sin(glfwGetTime()) * radius;
    const float camZ = cos(glfwGetTime()) * radius;

    memcpy(light.position, (vec3){camX, 0.0f, camZ}, sizeof(vec3));
	memcpy(data_src.position, light.position, sizeof(vec3));
    memcpy(data_target.position, &cube_positions[0], sizeof(vec3));

    //vec3 light_diffuse = {camX, 1.0f, camZ};
    //memcpy(light.diffuse, light_diffuse, sizeof(vec3));

	vec3 scale_f_src = {0.5, 0.5, 0.5};
	vec3 scale_f_target = {1.5, 1.5, 1.5};
	data_src.scaleFactor = &scale_f_src;
	data_target.scaleFactor = &scale_f_target;

    renderable_up_shader_uni_data(&src, &data_src);
    renderable_up_shader_uni_data(&target, &data_target);

    renderable_draw(&src);
    renderable_draw(&target);
}
