#pragma once
#include "Vertex.h"
#include "Light.h"
#include "Material.h"
#include <stdbool.h>
#include <cglm/call.h>

struct Shader;
struct LightVector;

typedef void(*uniform_calc_fp)(struct Shader*, void* uniform_calc_data);

typedef struct Shader
{
    unsigned id;
    bool valid;
    uniform_calc_fp uniform_calc_fp;
    void* uniform_calc_data;
} Shader;

bool shader_init(Shader* shader, const char* v_shader_path, const char* frag_shader_path);
bool shader_init_from_sources(Shader* shader, const char* v_shader_src, const char* frag_shader_src);
bool shader_init_with_f_and_data(Shader* shader,
                              const char* v_shader_path,
                              const char* frag_shader_path,
                              uniform_calc_fp calculate_fun,
                              void* calc_data);

void shader_setUniformf(Shader*, const char* name, float value);
void shader_setUniformi(Shader*, const char* name, int value);
void shader_setUniform4mat(Shader*, const char* name, mat4* value);
void shader_setUniform3mat(Shader*, const char* name, mat3* value);
void shader_setUniform3vec(Shader*, const char* name, const float value[]);

void shader_update_uniform_calc_data(Shader*, void* uniform_calc_data);
void shader_calculate_uniforms(Shader*);
void shader_use(Shader*);
void shader_set_light(Shader*, const Light*, const char* name);

void shader_set_lights(Shader* shader, const struct LightVector* vec);

// should shader know about material? I guess they are linked in many ways, so why not?
void shader_set_material(Shader* shader, const Material* material, const char* uniform_name, int offset);
