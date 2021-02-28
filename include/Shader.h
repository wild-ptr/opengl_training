#pragma once
#include "Vertex.h"
#include <stdbool.h>
#include <cglm/call.h>

typedef struct Shader
{
    unsigned id;
    bool valid;
} Shader;

bool shader_init(Shader* shader, const char* v_shader_path, const char* frag_shader_path);
bool shader_init_from_sources(Shader* shader, const char* v_shader_src, const char* frag_shader_src);
void shader_setUniformf(Shader*, const char* name, float value);
void shader_setUniformi(Shader*, const char* name, int value);
void shader_setUniform4f(Shader*, const char* name, Float4Vec value);
void shader_setUniform4mat(Shader*, const char* name, mat4* value);
void shader_use(Shader*);
