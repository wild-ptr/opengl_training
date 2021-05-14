#pragma once
#include <stddef.h>

#include "Renderable.h"
#include "Shader.h"


// TODO: make this shit dynamic jesus christ
#define MODEL_MAX_MESHES 1500

typedef struct Model
{
    Renderable meshes[MODEL_MAX_MESHES];
    const char* directory;
    size_t meshes_size;
    Shader* shader;
} Model;

void model_load(Model* model, const char* path, Shader* shader);
void model_draw(Model* model);

void model_bind_shader(Model* model, Shader* shader);
void model_bind_shader_data(Model* model, void* data);
