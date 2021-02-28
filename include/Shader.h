#pragma once
#include "Vertex.h"
#include <stdbool.h>
#include <cglm/call.h>

struct Shader;
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
void shader_setUniform4f(Shader*, const char* name, Float4Vec value);
void shader_setUniform4mat(Shader*, const char* name, mat4* value);
void shader_update_uniform_calc_data(Shader*, void* uniform_calc_data);
void shader_calculate_uniforms(Shader*);
void shader_use(Shader*);
