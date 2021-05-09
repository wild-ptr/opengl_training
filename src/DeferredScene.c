#include <cglm/call.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>

#include "Utilities.h"
#include "DeferredScene.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderable.h"
#include "Camera.h"
#include "GBuffer.h"
#include "utils/LightVector.h"

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
    { 2.0f,  5.0f, -15.0f},
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
    vec3 position;
    Camera* camera;
	vec3 scaleFactor;
    LightVector* lv;
} UniformData;

static void mvp_transform(Shader* shader, const UniformData* data)
{
    Camera* cam = data->camera;

    // simple model transform
    mat4 model_m = GLM_MAT4_IDENTITY_INIT;
    glmc_translate(model_m, data->position);
	glmc_scale(model_m, data->scaleFactor);
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
    shader_setUniformf(shader, "time", glfwGetTime());
}


static void calcUniforms_forLightSource(Shader* shader, void* raw_data)
{
    UniformData* data = raw_data;
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

typedef struct SceneLights
{
    LightVector lv;
    LightSpotlight* flashlight;
    Light* orb;
} SceneLights;

static SceneLights create_lights()
{
    SceneLights lights = {};

    LightVector lv = light_vector_init();

    Light omnilight = { .light_type = LIGHT_OMNIDIR,
                        .position = {2.0f, 1.0f, 2.0f},

                        .light_color = {.ambient = {0.1, 0.1, 0.1},
                                        .diffuse = {0.6, 0.6, 0.6},
                                        .specular = {0.3, 0.3, 0.3}},

                        .att_params = {.att_constant = 1.0,
                                       .att_linear = 0.014,
                                       .att_quadratic = 0.0007}
                        };

    lv_push_back(&lv, &omnilight);
    lights.orb = LV_AT(lv, 0);

    LightSpotlight spotlight = {.light_type = LIGHT_SPOTLIGHT,
                                .position = {2.0f, 1.0f, 2.0f},
                                .direction = {0.0f, 0.0f, -1.0f},

                                .inner_cone_cosine = cos(0.223599),
                                .outer_cone_cosine = cos(0.31),

                                .light_color = {.ambient = {0.1, 0.1, 0.1},
                                                .diffuse = {0.6, 0.6, 0.6},
                                                .specular = {0.3, 0.3, 0.3}},

                                .att_params = {.att_constant = 1.0,
                                               .att_linear = 0.045,
                                               .att_quadratic = 0.0075}
                                };

    lv_push_back(&lv, &spotlight);
    lights.flashlight = LV_AT(lv, 1);
    lights.lv = lv;

    return lights;
}

static Renderable create_light_target(Shader* shader)
{
    Material cube_materials[2] = {
        { .shininess = 32.0f },
        { .shininess = 32.0f }
    };

    material_init(&cube_materials[0],
                  "assets/bricks.jpg", GL_RGB,
                  "assets/container2_specular.png", GL_RGBA,
                  "assets/normal_bricks.jpg", GL_RGB);

    material_init(&cube_materials[1],
                  "assets/matrix.jpg", GL_RGB,
                  NULL, GL_RGB,
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
                      shader);

	return light_target;
}

static Renderable create_light_source()
{
	static Shader shader;
    CALL_ONCE
    {
    shader_init_with_f_and_data(&shader,
                                "src/shaders/LightningVShader.glsl",
                                "src/shaders/LightSourceFShader.glsl",
                                &calcUniforms_forLightSource,
                                NULL);
    CALL_ONCE_END;
    }

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

	return light_source;
}

static void update_flashlight_pos_to_cam(Camera* camera, LightSpotlight* flashlight)
{
    memcpy(flashlight->position, camera->pos_v, sizeof(vec3));
    memcpy(flashlight->direction, camera->dir_v, sizeof(vec3));
}

static void update_orb_light_and_src_pos(SceneLights* lights, UniformData* data_src)
{
    const float radius = 3.0f;
    const float camX = sin(glfwGetTime()) * radius;
    const float camZ = cos(glfwGetTime()) * radius;

    memcpy(lights->orb->position, (vec3){camX, 0.0f, camZ}, sizeof(vec3));
    memcpy(data_src->position, lights->orb->position, sizeof(vec3));
}

// This is super ugly and belongs in drawDeferredScene.
static SceneLights lights;
static bool flashlight_on = true;

// this is ugly and just for debugging.
void deferred_scene_specific_input_callback(int key, int action)
{
    // i need to get lights here for flashlight toggle
    if(key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        if(flashlight_on)
        {
            memcpy(lights.flashlight->light_color.diffuse, (vec3){0.0, 0.0, 0.0}, sizeof(vec3));
            flashlight_on = false;
        }
        else
        {
            memcpy(lights.flashlight->light_color.diffuse, (vec3){0.6, 0.6, 0.6}, sizeof(vec3));
            flashlight_on = true;
        }
    }
}

void drawDeferredScene(Camera* camera)
{
    static Renderable src;
    static Renderable targets[10];

    static GBuffer gbuffer;
    static Shader light_target_shader;

    static UniformData data_src = {.scaleFactor = {0.5, 0.5, 0.5}};
	static UniformData data_target = {.scaleFactor = {1.5, 1.5, 1.5}};

    // statics init
    CALL_ONCE
    {
        shader_init_with_f_and_data(
            &light_target_shader,
            "src/shaders/LightningVShader.glsl",
            "src/shaders/DeferredShaderFrag.glsl",
            &calcUniforms_forLightTarget,
            NULL);

		src = create_light_source();
        for(int i = 0; i < 10; ++i)
		    targets[i] = create_light_target(&light_target_shader);

        gbuffer_init(&gbuffer, 1500, 1200);
        lights = create_lights();
        CALL_ONCE_END;
    }

    data_src.camera = camera;
	data_target.camera = camera;

    update_flashlight_pos_to_cam(camera, lights.flashlight);
    update_orb_light_and_src_pos(&lights, &data_src);

    shader_set_lights(&light_target_shader, &lights.lv);

    for(int i = 0; i < 10; ++i)
    {
        memcpy(data_target.position, &cube_positions[i], sizeof(vec3));
        renderable_up_shader_uni_data(&targets[i], &data_target);
        renderable_draw(&targets[i]);
    }

    renderable_up_shader_uni_data(&src, &data_src);
    renderable_draw(&src);
}
